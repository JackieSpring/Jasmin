
#include "interpreter.h"


struct jin_interpreter {
    register_list * regs;
    instruction_set * isa;
    memory_map * mem;
    symbol_table * symt;
    
    jin_arch arch;
    jin_mode mode;
    jin_syntax syntax;
    ks_engine * ks;
    csh cs;
    
    void * op_res;  // operand_resolver
    void * ip_reader; // instruction pointer 
    void * ip_writer;
    void * sp_reader;  // stack pointer
    void * sp_writer;
    
    memory_addr entrypoint;
    memory_addr stackbase;
    segment_id working_segment;
    jin_state state;
};



/*
 * keystone let us resolve symbols that it can't resolve using a function so defined:
 *  ks_sym_resolver (* fun) (const char * sym, uint64_t * value)
 *
 * the problem with this method is that we cannot pass the interpreter to it, because 
 * of this the function cannot access to the symbol table of the interpreter. We 
 * need it only when keystone is assembling code, so as a workaround I use a global
 * variable that holds the interpreter passed to the wrapper functions as jin_assemble.
*/
static jin_interpreter * active_interpreter;



static ks_arch jin_to_ks_arch( jin_arch arch ){
    ks_arch ret;
    switch( arch ){
        case JIN_ARCH_ARM:      ret = KS_ARCH_ARM; break;
        case JIN_ARCH_ARM64:    ret = KS_ARCH_ARM64; break;
        case JIN_ARCH_MIPS:     ret = KS_ARCH_MIPS; break;
        case JIN_ARCH_X86:      ret = KS_ARCH_X86; break;
        case JIN_ARCH_PPC:      ret = KS_ARCH_PPC; break;
        case JIN_ARCH_SPARC:    ret = KS_ARCH_SPARC; break;
        case JIN_ARCH_EVM:      ret = KS_ARCH_EVM; break;
        default: ret = KS_ARCH_MAX;
    }
    
    return ret;
}

static ks_opt_value jin_to_ks_syntax( jin_syntax syn ) {
    ks_opt_value ret;
    switch( syn ) {
        case JIN_SYNTAX_INTEL:  ret = KS_OPT_SYNTAX_INTEL; break;
        case JIN_SYNTAX_ATT:    ret = KS_OPT_SYNTAX_ATT; break;
        case JIN_SYNTAX_NASM:   ret = KS_OPT_SYNTAX_NASM; break;
        case JIN_SYNTAX_MASM:   ret = KS_OPT_SYNTAX_MASM; break;
        case JIN_SYNTAX_GAS:    ret = KS_OPT_SYNTAX_GAS; break;
        default: 
                ret = KS_OPT_SYNTAX_INTEL;
    }

    return ret;
}

static cs_arch jin_to_cs_arch( jin_arch  arch ) {
    cs_arch ret;
    switch( arch ){
        case JIN_ARCH_ARM:      ret = CS_ARCH_ARM; break;
        case JIN_ARCH_ARM64:    ret = CS_ARCH_ARM64; break;
        case JIN_ARCH_MIPS:     ret = CS_ARCH_MIPS; break;
        case JIN_ARCH_X86:      ret = CS_ARCH_X86; break;
        case JIN_ARCH_PPC:      ret = CS_ARCH_PPC; break;
        case JIN_ARCH_SPARC:    ret = CS_ARCH_SPARC; break;
        case JIN_ARCH_EVM:      ret = CS_ARCH_EVM; break;
        default: ret = CS_ARCH_MAX;
    }
    
    return ret;
}

static cs_mode jin_to_ks_mode ( jin_mode mode ){
    ks_mode ret;
    switch(mode){
    case JIN_MODE_LITTLE_ENDIAN:    ret = KS_MODE_LITTLE_ENDIAN; break;
    case JIN_MODE_BIG_ENDIAN:       ret = KS_MODE_BIG_ENDIAN; break;
    // arm / arm64
    case JIN_MODE_ARM:    ret = KS_MODE_ARM; break;
    case JIN_MODE_THUMB:    ret = KS_MODE_THUMB; break;
    case JIN_MODE_V8:    ret = KS_MODE_V8; break;
    // mips
    //case JIN_MODE_MICRO:    ret = KS_MODE_MICRO; break;
    case JIN_MODE_MIPS3:    ret = KS_MODE_MIPS3; break;
    //case JIN_MODE_MIPS32R6:    ret = KS_MODE_MIPS32R6; break;
    //case JIN_MODE_MIPS32:    ret = KS_MODE_MIPS32; break;
    //case JIN_MODE_MIPS64:    ret = KS_MODE_MIPS64; break;
    // x86 / x64
    case JIN_MODE_16:    ret = KS_MODE_16; break;
    case JIN_MODE_32:    ret = KS_MODE_32; break;
    case JIN_MODE_64:    ret = KS_MODE_64; break;
    // ppc
    //case JIN_MODE_QPX:    ret = KS_MODE_QPX; break;
    // sparc
    //case JIN_MODE_V9:    ret = KS_MODE_V9; break;
    }
    
    return ret;
}

static cs_mode jin_to_cs_mode ( jin_mode mode ){
    cs_mode ret;
    switch(mode){
    case JIN_MODE_LITTLE_ENDIAN:    ret = CS_MODE_LITTLE_ENDIAN; break;
    case JIN_MODE_BIG_ENDIAN:       ret = CS_MODE_BIG_ENDIAN; break;
    // arm / arm64
    case JIN_MODE_ARM:    ret = CS_MODE_ARM; break;
    case JIN_MODE_THUMB:    ret = CS_MODE_THUMB; break;
    case JIN_MODE_V8:    ret = CS_MODE_V8; break;
    // mips
    //case JIN_MODE_MICRO:    ret = CS_MODE_MICRO; break;
    case JIN_MODE_MIPS3:    ret = CS_MODE_MIPS3; break;
    //case JIN_MODE_MIPS32R6:    ret = CS_MODE_MIPS32R6; break;
    //case JIN_MODE_MIPS32:    ret = CS_MODE_MIPS32; break;
    //case JIN_MODE_MIPS64:    ret = CS_MODE_MIPS64; break;
    // x86 / x64
    case JIN_MODE_16:    ret = CS_MODE_16; break;
    case JIN_MODE_32:    ret = CS_MODE_32; break;
    case JIN_MODE_64:    ret = CS_MODE_64; break;
    // ppc
    //case JIN_MODE_QPX:    ret = CS_MODE_QPX; break;
    // sparc
    //case JIN_MODE_V9:    ret = CS_MODE_V9; break;
    }
    
    return ret;
}

static cs_opt_value jin_to_cs_syntax ( jin_syntax syn ){
    cs_opt_value ret;
    switch( syn ) {
        case JIN_SYNTAX_INTEL:  ret = CS_OPT_SYNTAX_INTEL; break;
        case JIN_SYNTAX_ATT:    ret = CS_OPT_SYNTAX_ATT; break;
        case JIN_SYNTAX_NASM:   ret = CS_OPT_SYNTAX_MASM; break;
        case JIN_SYNTAX_MASM:   ret = CS_OPT_SYNTAX_MASM; break;
        case JIN_SYNTAX_GAS:    ret = CS_OPT_SYNTAX_ATT; break;
        default: 
                ret = CS_OPT_SYNTAX_INTEL;
    }

    return ret;
}



static ks_sym_resolver symres(const char * sym, uint64_t * value){
    uint64_t addr;
    if( get_symbol_value( active_interpreter->symt, sym, &addr ) < 0)
        return false;
    *value = addr;
        
    return true;
}


/*
 * !!!!!!!!
 * Ancora non supporta la creazione di segmenti arbitrari, includere questa funzione il prima possibile
*/
jin_interpreter * jin_init_interpreter( jin_options * jops ) {

    if ( jops == NULL )
        goto cleanup;

    jin_arch arch = jops->arch;
    jin_mode mode = jops->mode;
    jin_syntax syn = jops->syntax;

    ks_engine * ks;
    csh cs;
    ks_err kerr;
    cs_err cerr;
    
    jin_interpreter * ret = calloc(1, sizeof(jin_interpreter));
    
    if ( ret == NULL)
        goto cleanup;
        
    ret->arch = arch;
    ret->mode = mode;
    ret->syntax = syn;

    /**
    * NO jin_option->entrypoint still exists,  so entrypoint is static.
    * TODO: Modify entrypoint initialization depending on jin_options
    *       entrypoint init shall be done here or in arch_init() functions?  
    * The 32 and 64 bit modes have the same id in all architectures.
    */
    if ( mode & JIN_MODE_32 ){
        ret->entrypoint = MEM_TEXT_DEFAULT_ADDRESS_32;
        ret->stackbase  = MEM_STACK_DEFAULT_ADDRESS_32;
    }       
    else if ( mode & JIN_MODE_64 ){
        ret->entrypoint = MEM_TEXT_DEFAULT_ADDRESS_64;
        ret->stackbase  = MEM_STACK_DEFAULT_ADDRESS_64;
    }
    
// init keystone
    kerr = ks_open( jin_to_ks_arch(arch) , jin_to_ks_mode(mode) , &ks );
    ret->ks = ks;
    if ( kerr != KS_ERR_OK )
        goto cleanup;
    
    ks_option(ret->ks, KS_OPT_SYM_RESOLVER, symres);
    ks_option(ret->ks, KS_OPT_SYNTAX, jin_to_ks_syntax(syn) );

        
// init capstone
    cerr = cs_open(jin_to_cs_arch(arch) , jin_to_cs_mode(mode), &cs);
    ret->cs = cs;
    if ( cerr != CS_ERR_OK )
        goto cleanup;
    
    cs_option(ret->cs, CS_OPT_DETAIL, CS_OPT_ON);
    cs_option( ret->cs, CS_OPT_SYNTAX, jin_to_cs_syntax(syn) );
        

// init memory

    if ( mode == JIN_MODE_32 )
        ret->mem = create_memory_map( MEM_32 );
    else
        ret->mem = create_memory_map( MEM_64 );
    
    if( ret->mem == NULL )
        goto cleanup;
    
    memory_addr start_addr = ( mode == JIN_MODE_32 ) ? MEM_TEXT_DEFAULT_ADDRESS_32 : MEM_TEXT_DEFAULT_ADDRESS_64;
    
    add_segment_to_map (ret->mem, MEM_SEG_TEXT, start_addr, MEM_TEXT_DEFAULT_SIZE , MEM_EXEC | MEM_READ, false );

    start_addr += MEM_TEXT_DEFAULT_SIZE;
    add_segment_to_map (ret->mem, MEM_SEG_RODATA, start_addr, MEM_RODATA_DEFAULT_SIZE, MEM_READ, false );

    start_addr += MEM_RODATA_DEFAULT_SIZE;
    add_segment_to_map (ret->mem, MEM_SEG_DATA, start_addr, MEM_DATA_DEFAULT_SIZE , MEM_READ | MEM_WRITE, false );

    start_addr += MEM_DATA_DEFAULT_SIZE;
    add_segment_to_map (ret->mem, MEM_SEG_BSS, start_addr, MEM_BSS_DEFAULT_SIZE , MEM_READ | MEM_WRITE, false );

    start_addr = ( mode == JIN_MODE_32 ) ? MEM_STACK_DEFAULT_ADDRESS_32 : MEM_STACK_DEFAULT_ADDRESS_64;
    add_segment_to_map (ret->mem, MEM_SEG_STACK, start_addr, MEM_STACK_DEFAULT_SIZE , MEM_READ | MEM_WRITE, true );

// init symbols
    ret->symt = create_symbol_table();
    if ( ret->symt == NULL )
        goto cleanup;
    
    add_symbol_to_table(ret->symt, MEM_SEG_SYM_TEXT, offset_read_from_segment(ret->mem, MEM_SEG_TEXT,0, NULL, 0) );
    add_symbol_to_table(ret->symt, MEM_SEG_SYM_RODATA, offset_read_from_segment(ret->mem, MEM_SEG_RODATA,0, NULL, 0) );
    add_symbol_to_table(ret->symt, MEM_SEG_SYM_DATA, offset_read_from_segment(ret->mem, MEM_SEG_DATA,0, NULL, 0) );
    add_symbol_to_table(ret->symt, MEM_SEG_SYM_BSS, offset_read_from_segment(ret->mem, MEM_SEG_BSS,0, NULL, 0) );
    add_symbol_to_table(ret->symt, MEM_SEG_SYM_STACK, offset_read_from_segment(ret->mem, MEM_SEG_STACK,0, NULL, 0) );

// init architecture
    switch( arch ){
        case JIN_ARCH_X86:
            init_x86(ret);
            break;
        
        case JIN_ARCH_ARM:
        case JIN_ARCH_ARM64:
        case JIN_ARCH_EVM:
        case JIN_ARCH_MIPS:
        case JIN_ARCH_PPC:
        case JIN_ARCH_SPARC:
            puts("Architecture not supported yet!");
            goto cleanup;
            break;
        default:
            puts("Unsupported architecture!");
            goto cleanup;
            break;
    }

    return ret;

cleanup:
    if ( ret != NULL ) {
        if ( ret->regs != NULL )
            destroy_register_list(ret->regs);
        
        if (ret->isa != NULL)
            destroy_instruction_set(ret->isa);
        
        if ( ret->symt != NULL )
            destroy_hashtable(ret->symt);
        
        if ( ret->mem != NULL )
            destroy_memory_map(ret->mem);
        
        if ( ret->ks != NULL )
            ks_close(ret->ks);
        
        if ( ret->cs != NULL )
            cs_close(ret->cs);
        
        free(ret);
    }
    
    return NULL;
}

void jin_fini_interpreter(jin_interpreter * ret) {
    if ( ret != NULL ) {
        if ( ret->regs != NULL )
            destroy_register_list(ret->regs);
        
        if ( ret->isa != NULL)
            destroy_instruction_set(ret->isa);
        
        if ( ret->mem != NULL )
            destroy_memory_map(ret->mem);
        
        if ( ret->ks != NULL )
            ks_close(ret->ks);
        
        if ( ret->cs != NULL )
            cs_close(&(ret->cs));
        
        if ( ret->symt != NULL )
            destroy_symbol_table(ret->symt);
        
        free(ret);
    }
}

/* ############################################################################
 *                      INTERPRETER UTILITY FUNCTIONS
 * ############################################################################
*/
jin_arch jin_get_arch(jin_interpreter * jint){
    return jint->arch;
}

jin_mode jin_get_mode(jin_interpreter * jint){
    return jint->mode;
}

jin_syntax jin_get_syntax( jin_interpreter * jint ){
    return jint->syntax;
}

int jin_set_state (jin_interpreter * jint, jin_state state) {
    jint->state = state;
    return 0;
}
jin_state jin_get_state(jin_interpreter * jint) {
    return jint->state;
}


/* ############################################################################
 *                      ARCHITECTURE-SPECIFIC FUNCTIONS ACCESS
 * ############################################################################
*/
int resolve_operands (jin_interpreter * jint, cs_insn * raw_ins , jin_operand ** ret, size_t ** ret_op_count ) {
    return ((operand_resolver)jint->op_res)( jint, raw_ins, ret, ret_op_count );
}

register_size read_instruction_pointer  (jin_interpreter * jint, void * buffer) {
    return ( (register_access_function ) jint->ip_reader ) (jint, buffer);
}
register_size write_instruction_pointer (jin_interpreter * jint, void * buffer) {
    return ( (register_access_function ) jint->ip_writer ) (jint, buffer);
}

register_size read_stack_pointer ( jin_interpreter *  jint, void * buffer ) {
    return ( (register_access_function ) jint->sp_reader ) (jint, buffer);
}
register_size write_stack_pointer ( jin_interpreter *  jint, void * buffer ){
    return ( (register_access_function ) jint->sp_writer ) (jint, buffer);
}



int jin_set_operand_resolver( jin_interpreter * jint, operand_resolver func ) {
    if ( jint == NULL || func == NULL )
        return -1;
    
    jint->op_res = func;
    return 0;
}

    // instruction and stack pointer access 
int jin_set_instruction_pointer_rw ( jin_interpreter * jint, register_access_function rdf, register_access_function wrf ) {
    if ( jint == NULL || rdf == NULL || wrf == NULL )
        return -1;
    
    jint->ip_reader = rdf;
    jint->ip_writer = wrf;
    
    memory_addr text_addr = offset_read_from_segment(jint->mem, MEM_SEG_TEXT,  0, NULL, 0 );
    wrf( jint, &text_addr );
    
    return 0;
}

int jin_set_stack_pointer_rw ( jin_interpreter * jint, register_access_function rdf, register_access_function wrf ){
    if ( jint == NULL || rdf == NULL || wrf == NULL )
        return -1;
    
    jint->sp_reader = rdf;
    jint->sp_writer = wrf;
    
    memory_addr stack_addr = offset_read_from_segment(jint->mem, MEM_SEG_STACK,  0, NULL, 0 );
    wrf( jint, &stack_addr );
    
    return 0;
}



/* ############################################################################
 *                              RESOURCES ACCESS
 * ############################################################################
*/
/*
 * SYMBOLS HANDLERS
*/
int jin_add_symbol( jin_interpreter * jint , symbol_key sk, symbol_value val){
    return add_symbol_to_table( jint->symt, sk, val);
}
int jin_del_symbol( jin_interpreter * jint , symbol_key sk){
    return del_symbol_from_table( jint->symt, sk);
}

int jin_get_symbol( jin_interpreter * jint, symbol_key sk, symbol_value * ret ){
    return get_symbol_value( jint->symt, sk,  ret );
}


/*
 * REGISTERS HANDLERS
*/
int jin_create_register_table(jin_interpreter * jint, register_id size){
    if ( jint == NULL )
        goto cleanup;
        
    jint->regs = create_register_list( size );
    
    if ( jint->regs == NULL )
        goto cleanup;
    
    return 0;

cleanup:
    return -1;
}

int jin_create_instruction_table(jin_interpreter * jint, instruction_id size){
    if ( jint == NULL )
        goto cleanup;
        
    jint->isa = create_instruction_set( size );
    
    if ( jint->isa == NULL )
        goto cleanup;
    
    return 0;

cleanup:
    return -1;
}

int jin_add_register(jin_interpreter * jin, register_id id, register_size size){
    return add_register_to_list(jin->regs, id, size);
}
int jin_add_register_partition( jin_interpreter * jint, register_id partid, register_id parentid, register_size offset, register_size size ) {
    return add_register_partition_to_list(jint->regs, partid, parentid, offset, size);
}
int jin_del_register(jin_interpreter * jin, register_id id ){
    return del_register_from_list(jin->regs, id);
}

int jin_add_instruction( jin_interpreter * jint, instruction_id id, instruction_handler func ){
    return add_instruction_to_set  ( jint->isa,  id, (instruction_function) func );
}
int jin_del_instruction( jin_interpreter * jint, instruction_id id ){
    return del_instruction_from_set  ( jint->isa,  id );
}




register_size read_register( jin_interpreter * jint, register_id regid, void * buffer) {
    return read_from_register(jint->regs, regid, buffer);
}

register_size write_register( jin_interpreter * jint, register_id regid, void * buffer){
    return write_into_register (jint->regs, regid, buffer);
}

/*
 * MEMORY HANDLERS
*/
size_t read_memory ( jin_interpreter * jint, memory_addr addr, void * buffer, size_t length ){
    return read_from_memory( jint->mem, addr, buffer, length );
}

size_t write_memory ( jin_interpreter * jint, memory_addr addr, void * buffer, size_t length ){
    return write_to_memory( jint->mem, addr, buffer, length );
}

size_t force_read_memory ( jin_interpreter * jint, memory_addr addr, void * buffer, size_t length ){
    return force_read_from_memory( jint->mem, addr, buffer, length );
}

size_t force_write_memory ( jin_interpreter * jint, memory_addr addr, void * buffer, size_t length ){
    return force_write_to_memory( jint->mem, addr, buffer, length );
}


void jin_free_instruction( jin_instruction * ins ){
    if ( ins == NULL )
        return;
    
    if ( ins->op_count > 0 )
        free(ins->op);
    
    free(ins);
}

/*
 * ITERATORS
*/

int jin_iterate_register(jin_interpreter * jint, register_iterator riter, void * extra ) {
    return iter_register_list( jint->regs, riter, extra );
}

int jin_iterate_symbol(jin_interpreter * jint, symbol_iterator siter, void * extra ) {
    return iter_symbol_table( jint->symt, siter, extra );
}


int jin_iterate_memory ( jin_interpreter * jint, memory_iterator miter, void * extra) {
    return iter_memory_map(jint->mem, miter, extra);
}



const char * jin_get_reg_name( jin_interpreter * jint, register_id id ) {
    return cs_reg_name( jint->cs, id );
}


/*
 * 
*/

jin_err jin_assemble( jin_interpreter * jint, unsigned char * asm_code, uint64_t ins_p, uint8_t ** bytecode, size_t * bytecode_size, size_t * ins_count ) {
    if( jint == NULL )
        return JIN_ERR_GENERIC;
    
    jin_err jerr = JIN_ERR_OK;
    active_interpreter = jint;
    
    if( ks_asm(jint->ks, asm_code, ins_p, bytecode, bytecode_size, ins_count ) != KS_ERR_OK )
        jerr = ks_to_jin_err( ks_errno(jint->ks) );
    
    if ( jerr == JIN_ERR_OK && (*bytecode_size == 0 || *ins_count == 0 ))         // keystone false positive
        jerr = JIN_ERR_ASM_MNEMONICFAIL ;
    
    return jerr;
}

jin_err jin_disassemble( jin_interpreter * jint, uint64_t ins_p, uint8_t * bytecode, size_t size, jin_instruction ** ret_insn_p ) {
    if( jint == NULL || bytecode == NULL || ret_insn_p == NULL )
        return JIN_ERR_GENERIC;
    
    
    cs_insn * raw_insn = cs_malloc(jint->cs);
    jin_instruction * ret = NULL;
    jin_err jerr;
    
    if ( raw_insn == NULL ) {
        jerr = JIN_ERR_MEMORY;
        goto cleanup;
    }

    if ( cs_disasm_iter(jint->cs, &bytecode, &size, &ins_p, raw_insn ) == false ) {
        jerr = cs_to_jin_err( cs_errno(jint->cs) );
        goto cleanup;
    }

    
    ret = calloc( 1 , sizeof(jin_instruction));
    if ( ret == NULL ) {
        jerr = JIN_ERR_MEMORY;
        goto cleanup;
    }
    
    ret->id = raw_insn->id;
    ret->byte_size = raw_insn->size;
    
    strcpy( ret->ins_str, raw_insn->mnemonic);
    strcpy( ret->op_str, raw_insn->op_str );
    
    if( resolve_operands (jint, raw_insn, &ret->op, &ret->op_count ) != JIN_ERR_OK ) {
        jerr = JIN_ERR_OPERAND_FAIL;
        goto cleanup;
    }
    
    *ret_insn_p = ret;
    
    cs_free(raw_insn, 1);
    
    return JIN_ERR_OK;

cleanup:
    if ( raw_insn )
        cs_free(raw_insn, 1);
    
    if ( ret != NULL )
        jin_free_instruction(ret);
        
    return jerr;
}

jin_err jin_execute( jin_interpreter * jint, jin_instruction * ins ){
    if ( jint == NULL || ins == NULL )
        return JIN_ERR_GENERIC;
    
    instruction_handler handler = (instruction_handler *)get_instruction_function( jint->isa, ins->id ) ;
    if ( handler == NULL )
        return JIN_ERR_INSN_NOTSUPPORTED;
    
    return (handler)( jint, ins->op, ins->op_count );
}


memory_addr offset_write_segment ( jin_interpreter * jint , segment_id id, size_t offset, void * buffer, size_t length ) {
    return offset_write_to_segment(jint->mem, id, offset, buffer, length);
}
memory_addr offset_read_segment ( jin_interpreter * jint, segment_id id, size_t offset, void * buffer, size_t length ) {
    return offset_read_from_segment(jint->mem, id, offset, buffer, length);
}
memory_addr push_segment ( jin_interpreter * jint, segment_id id, void * buffer, size_t length ) {
    return push_to_segment(jint->mem, id, buffer, length);
}
memory_addr pop_segment ( jin_interpreter * jint, segment_id id, void * buffer, size_t length ) {
    return pop_from_segment(jint->mem, id, buffer, length);
}

bool check_perm_memory( jin_interpreter * jint, memory_addr addr, memory_perm perm ){
    return check_address_perm(jint->mem, addr, perm );
}

void * get_effective_pointer( jin_interpreter * jint, memory_addr address ){
    return get_real_memory_pointer( jint->mem, address );
}

int jin_set_working_segment( jin_interpreter * jint , segment_id id ) {
    jint->working_segment = id;
    return 0;
}
segment_id jin_get_working_segment( jin_interpreter * jint ){
    return jint->working_segment;
}

int jin_set_entrypoint( jin_interpreter * jint, memory_addr addr ) {
    jint->entrypoint = addr;
    return 0;
}
memory_addr jin_get_entrypoint( jin_interpreter * jint ) {
    return jint->entrypoint;
}

int jin_set_stackbase( jin_interpreter * jint, memory_addr addr ) {
    jint->stackbase = addr;
    return 0;
}
memory_addr jin_get_stackbase( jin_interpreter * jint ) {
    return jint->stackbase;
}


string jin_state_to_string( jin_state state ){
    string ret;

    switch( state ) {
        case JIN_STATE_RUNNING:     ret = "running"; break;
        case JIN_STATE_INACTIVE:    ret = "inactive"; break;
        case JIN_STATE_PAUSE:       ret = "paused"; break;
        case JIN_STATE_TERMINATED:  ret = "terminated"; break;
        default:
            ret = "undefined";
    }

    return ret;
}

string jin_arch_to_string( jin_arch arch ){
    string ret;

    switch( arch ){
        case JIN_ARCH_ARM:      ret = "ARM"; break;
        case JIN_ARCH_ARM64:    ret = "ARM64"; break;
        case JIN_ARCH_MIPS:     ret = "MIPS"; break;
        case JIN_ARCH_X86:      ret = "X86"; break;
        case JIN_ARCH_PPC:      ret = "PPC"; break;
        case JIN_ARCH_SPARC:    ret = "SPARC"; break;
        case JIN_ARCH_EVM:      ret = "EVM"; break;
        default: ret = "undefined";
    }

    return ret;
}

string jin_mode_to_string(jin_arch arch, jin_mode mode ){
    string ret;

    if ( arch == JIN_ARCH_ARM ){
        if ( mode & JIN_MODE_ARM )          ret = "Arm32";
        else if ( mode & JIN_MODE_THUMB )   ret = "Thumb";
        else if ( mode & JIN_MODE_V8 )      ret = "V8";
    }
    else if ( arch == JIN_ARCH_X86 ){
        if ( mode & JIN_MODE_16 )           ret = "16bit";
        else if ( mode & JIN_MODE_32 )      ret = "32bit";
        else if ( mode & JIN_MODE_64 )      ret = "64bit";
    }
    else if ( arch == JIN_ARCH_MIPS ){
        if ( mode & JIN_MODE_MICRO )        ret = "Micro";
        else if ( mode & JIN_MODE_MIPS3 )   ret = "MipsS3";
        else if ( mode & JIN_MODE_MIPS32 )  ret = "Mips32";
        else if ( mode & JIN_MODE_MIPS64 )  ret = "Mips64";
    }
    else if ( arch == JIN_ARCH_PPC ){
        if ( mode & JIN_MODE_QPX )          ret = "Qpx";
    }
    else if ( arch == JIN_ARCH_SPARC ){
        if ( mode & JIN_MODE_V9 )           ret = "V9";
    }
    else{
        ret = "undefined";
    }

    return ret;
}

string jin_endianess_to_string(jin_mode mode){
    string ret;

    if ( mode & JIN_MODE_LITTLE_ENDIAN )    ret = "LittleEndian";
    else if( mode & JIN_MODE_BIG_ENDIAN )   ret = "BigEndian";
    else
        ret = "undefined";

    return ret;
}

string jin_syntax_to_string(jin_arch arch, jin_syntax syn){
    string ret;

    if ( arch == JIN_ARCH_X86 ){
        switch(syn){
            case JIN_SYNTAX_INTEL:  ret = "Intel"; break;
            case JIN_SYNTAX_ATT:    ret = "Att"; break;
            case JIN_SYNTAX_GAS:    ret = "Gas"; break;
            case JIN_SYNTAX_MASM:   ret = "Masm"; break;
            case JIN_SYNTAX_NASM:   ret = "Nasm"; break;
            default:
                ret = "undefined";
        }
    }
    else
        ret = "standard";

    return ret;
}