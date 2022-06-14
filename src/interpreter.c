
#include "interpreter.h"



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


/*
 * !!!!!!!!
 * Ancora non supporta la creazione di segmenti arbitrari, includere questa funzione il prima possibile
*/
jin_interpreter * jin_init_interpreter( jin_arch arch, jin_mode mode ) {
    ks_engine * ks;
    csh cs;
    ks_err kerr;
    cs_err cerr;
    
    jin_interpreter * ret = calloc(1, sizeof(jin_interpreter));
    
    if ( ret == NULL)
        goto cleanup;
        
    ret->arch = arch;
    ret->mode = mode;
// init keystone
    kerr = ks_open( jin_to_ks_arch(arch) , jin_to_ks_mode(mode) , &ks );
    ret->ks = ks;
    if ( kerr != KS_ERR_OK )
        goto cleanup;
// init capstone
    cerr = cs_open(jin_to_cs_arch(arch) , jin_to_cs_mode(mode), &cs);
    ret->cs = cs;
    if ( cerr != CS_ERR_OK )
        goto cleanup;
    
    cs_option(ret->cs, CS_OPT_DETAIL, CS_OPT_ON);
// init memory
    struct options opt = {
        .mode = MEM_64,
        
        .stack_start =  MEM_DEFAULT,
        .text_start = MEM_DEFAULT,
        .rodata_start = MEM_DEFAULT,
        .data_start = MEM_DEFAULT,
        .bss_start = MEM_DEFAULT,
        
        .stack_size = MEM_DEFAULT,
        .text_size = MEM_DEFAULT,
        .rodata_size = MEM_DEFAULT,
        .data_size = MEM_DEFAULT,
        .bss_size = MEM_DEFAULT,
    };
    
    if ( mode == JIN_MODE_32 )
        opt.mode = MEM_32;

    ret->mem = create_memory_map( &opt );
    if( ret->mem == NULL )
        goto cleanup;
// init symbols
    ret->symt = create_symbol_table();
    if ( ret->symt == NULL )
        goto cleanup;
    
    return ret;

cleanup:
    if ( ret != NULL ) {
        if ( ret->regs != NULL )
            destroy_register_list(ret->regs);
        
        if (ret->isa != NULL)
            destroy_instruction_set(ret->isa);
        
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
            
        jin_free_all(ret);
        
        free(ret);
    }
}


/*
 * Questa funzione serve a creare blocchi di memoria che vengono liberati alla distruzioner dell'interprete
*/
void * jin_malloc(jin_interpreter * jint, size_t size ) {
    if ( jint == NULL || size == 0)
        goto cleanup;
    
    jin_dynamic_memory * new_block = (jin_dynamic_memory *)calloc( 1, sizeof( jin_dynamic_memory ) + size );

    if ( new_block == NULL )
        goto cleanup;
    
    new_block->next = jint->jin_dyn_mem_blocks_list;
    jint->jin_dyn_mem_blocks_list = new_block;
    
    return ((void *)(new_block) + sizeof( jin_dynamic_memory * ));

cleanup:
    return NULL;
}

void jin_free_all(jin_interpreter * jint) {
    if ( jint == NULL )
        return;
    
    jin_dynamic_memory * node = jint->jin_dyn_mem_blocks_list;
    jin_dynamic_memory * old;
    
    while( node != NULL ){
        old = node;
        node = node->next;
        free(old);
    }
}



jin_arch jin_get_arch(jin_interpreter * jint){
    return jint->arch;
}

jin_mode jin_get_mode(jin_interpreter * jint){
    return jint->mode;
}

void jin_start_interpreter(jin_interpreter * jint) {
    jint->running = true;
}

void jin_stop_interpreter( jin_interpreter * jint ) {
    jint->running = false;
}

bool jin_is_running( jin_interpreter * jint ) {
    return jint->running;
}


// ARCH-SPECIFIC FUNCTIONS

int resolve_operands (jin_interpreter * jint, cs_insn * raw_ins , jin_operand ** ret, size_t ** ret_op_count ) {
    return ((operand_resolver)jint->op_res)( jint, raw_ins, ret, ret_op_count );
}

register_size read_instruction_pointer  (jin_interpreter * jint, void * buffer) {
    return ( (instruction_pointer_reader ) jint->ip_reader ) (jint, buffer);
}
register_size write_instruction_pointer (jin_interpreter * jint, void * buffer) {
    return ( (instruction_pointer_writer ) jint->ip_writer ) (jint, buffer);
}

register_size read_stack_pointer ( jin_interpreter *  jint, void * buffer ) {
    return ( (stack_pointer_reader ) jint->sp_reader ) (jint, buffer);
}
register_size write_stack_pointer ( jin_interpreter *  jint, void * buffer ){
    return ( (stack_pointer_writer ) jint->sp_writer ) (jint, buffer);
}



int jin_set_operand_resolver( jin_interpreter * jint, operand_resolver func ) {
    if ( jint == NULL || func == NULL )
        return -1;
    
    jint->op_res = func;
    return 0;
}

    // instruction and stack pointer access 
int jin_set_instruction_pointer_rw ( jin_interpreter * jint, instruction_pointer_reader rdf, instruction_pointer_writer wrf ) {
    if ( jint == NULL || rdf == NULL || wrf == NULL )
        return -1;
    
    jint->ip_reader = rdf;
    jint->ip_writer = wrf;
    
    memory_addr text_addr = offset_read_text( jint->mem, 0, NULL, 0 );
    wrf( jint, &text_addr );
    
    return 0;
}

int jin_set_stack_pointer_rw ( jin_interpreter * jint, stack_pointer_reader rdf, stack_pointer_writer wrf ){
    if ( jint == NULL || rdf == NULL || wrf == NULL )
        return -1;
    
    jint->sp_reader = rdf;
    jint->sp_writer = wrf;
    
    memory_addr stack_addr = offset_read_stack( jint->mem, 0, NULL, 0 );
    wrf( jint, &stack_addr );
    
    return 0;
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

int jin_add_register(jin_interpreter * jin, register_id id, register_size size, void * address){
    return add_register_to_list(jin->regs, id, size, address);
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



//
// WORKFLOW FUNCTIONS
//

void jin_free_instruction( jin_instruction * ins ){
    if ( ins == NULL )
        return;
    
    if ( ins->op_count > 0 )
        free(ins->op);
    
    free(ins);
}


