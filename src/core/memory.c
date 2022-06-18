#include "memory.h"

#define MEM_32_MAX_ADDRESS 0xffffffff
#define MEM_64_MAX_ADDRESS 0xffffffffffffffff

#define MEM_MODE_IS_32(mm) (mm->mode == MEM_32)
#define MEM_MODE_IS_64(mm) (mm->mode == MEM_64)
#define IS_VALID_ADDRESS(mm, addr) ( addr <= ( (MEM_MODE_IS_32(mm) ) ? MEM_32_MAX_ADDRESS : ((MEM_MODE_IS_64(mm) ) ? MEM_64_MAX_ADDRESS : 0 ) ) )

#define PERM_IS_EXEC(perm) ((perm & MEM_EXEC) != 0)
#define PERM_IS_WRITE(perm) ((perm & MEM_WRITE) != 0)
#define PERM_IS_READ(perm) ((perm & MEM_READ) != 0)


typedef struct segment_struct{
    char *     name;

    memory_addr    start_mem_address;
    memory_addr    end_mem_address;
    
    size_t     size;
    size_t      top;
    void *      pointer;
    
    memory_perm perm;
    bool        is_stack;
} segment_struct;
typedef segment_struct * segment;

struct memory_map_struct {
    size_t  nsegments;
    memory_mode mode;
    segment text;
    segment rodata;
    segment data;
    segment bss;
    segment stack;
    segment * segment_map;
};


//DEBUG
static void print_section(segment sec){
        printf("segment %s\n",      sec->name);
        printf("\tstart: %p\n",     (void *)sec->start_mem_address);
        printf("\tend: %p\n",       (void *)sec->end_mem_address);
        printf("\tsize: %p\n",      (void *)sec->size);
        printf("\tpointer: %p\n",   (void *)sec->pointer);
        printf("\tperm: %p\n",      (void *)sec->perm);
        printf("\tis_stack: %p\n",  (void *)sec->is_stack);
        putchar('\n');
}

static void print_sections(memory_map * mm){
    for ( int i = 0; i < mm->nsegments; i++ ) {
        segment sec = mm->segment_map[i];
        print_section(sec);
    }
}



static segment create_segment( memory_map * mm, char * name, memory_addr start_address, size_t size, memory_perm perm, bool is_stack ) {
    segment new_segment = calloc( sizeof( segment_struct ) ,1 );
    
    if ( new_segment == NULL )
        goto cleanup;
    
    #define end_addr start_address + size
    
    if ( ! IS_VALID_ADDRESS(mm, end_addr) )
        goto cleanup;
    
    new_segment->name = name;
    
    if ( is_stack == true ) {
        
        if ( size > start_address )
            goto cleanup;
        new_segment->start_mem_address = start_address - size;
        new_segment->end_mem_address = start_address ;
        new_segment->top = size;
    }
    else{
        // If start_address+size is bigger then the boundary, then the sum is going to be lesser then start_address
        if ( ( mm->mode == MEM_64 && start_address + size < start_address ) || ( mm->mode == MEM_32 && start_address + size < start_address ) )
            goto cleanup;
        
        new_segment->start_mem_address = start_address;
        new_segment->end_mem_address = start_address + size;
        new_segment->top = 0;
    }
    
    new_segment->pointer = calloc( size, 1 );
    if ( new_segment->pointer == NULL )
        goto cleanup;
    
    new_segment->is_stack = is_stack;
    new_segment->perm = perm;
    new_segment->size = size;

    #undef end_addr
    
    return new_segment;

cleanup:
    if (new_segment)
        free(new_segment);
    
    return NULL;
}

static void destroy_segment( segment old_segment ) {
    free(old_segment->pointer);
    free(old_segment);
}



static int segments_comparer( const void * p1, const void * p2 ) {
    segment sect1 = (segment) *((segment *)p1);
    segment sect2 = (segment) *((segment *)p2);
 
    if ( sect1->start_mem_address < sect2->start_mem_address )
        return -1;
    else if ( sect1->start_mem_address > sect2->start_mem_address )
        return 1;
    
    return 0;
}

static int address_in_segment( const void * addr_ptr, const void * sect_ptr ) {
    segment sect = (segment) *((segment *)sect_ptr);
    memory_addr addr = *((memory_addr *)addr_ptr);
    
    if ( addr < sect->start_mem_address )
        return -1;
    else if ( addr >= sect->end_mem_address )
        return 1;
    
    return 0;
}

static segment search_address_segment ( memory_map * mm, const memory_addr addr ){
    segment * sect_ptr = (segment *)bsearch( &addr, mm->segment_map, mm->nsegments, sizeof(segment), address_in_segment );
    
    if ( sect_ptr == NULL )
        return NULL;
    
    return * sect_ptr;
}


/*
 *
*/
memory_map * create_memory_map( struct options * opt ) {

    struct options fops = {0};
    volatile memory_addr sec_address;
    volatile size_t sec_size;
    segment temp;
    memory_map * ret = calloc(1, sizeof(memory_map) ) ;
    
    if ( ret == NULL )
        goto cleanup;
    
    if ( opt == NULL ) {
            fops.mode = MEM_64;
            fops.stack_start   = MEM_DEFAULT;
            fops.text_start    = MEM_DEFAULT;
            fops.rodata_start  = MEM_DEFAULT;
            fops.data_start    = MEM_DEFAULT;
            fops.bss_start     = MEM_DEFAULT;
            
            fops.stack_size    = MEM_DEFAULT;
            fops.text_size     = MEM_DEFAULT;
            fops.rodata_size   = MEM_DEFAULT;
            fops.data_size     = MEM_DEFAULT;
            fops.bss_size      = MEM_DEFAULT;
            opt = &fops;
    
    }
    
    ret->mode = opt->mode;
    ret->nsegments = 5;

// INITIALIZE segmentS

//text_init:
    sec_address = opt->text_start;
    sec_size = opt->text_size;
    
    if ( sec_address == MEM_DEFAULT ) {
        if ( ret->mode == MEM_32 )
            sec_address = TEXT_DEFAULT_ADDRESS_32;
        else if ( ret->mode == MEM_64 )
            sec_address = TEXT_DEFAULT_ADDRESS_64;
        else
            goto cleanup;
    }
    
    if ( sec_size == MEM_DEFAULT )
        sec_size = TEXT_DEFAULT_SIZE;
    
    if ((ret->text = create_segment( ret,"text", sec_address, sec_size, MEM_READ | MEM_EXEC , false)) == NULL )
        goto cleanup;

//rodata_init:
    sec_address = opt->rodata_start;
    sec_size = opt->rodata_size;
    
    if ( sec_address == MEM_DEFAULT )
        sec_address = ret->text->end_mem_address;
        
    if ( sec_size == MEM_DEFAULT )
        sec_size = RODATA_DEFAULT_SIZE;
    
    if ((ret->rodata = create_segment( ret,"rodata", sec_address, sec_size, MEM_READ, false)) == NULL )
        goto cleanup;

//data_init:
    sec_address = opt->data_start;
    sec_size = opt->data_size;
    
    if ( sec_address == MEM_DEFAULT )
        sec_address = ret->rodata->end_mem_address;
    
    if ( sec_size == MEM_DEFAULT )
        sec_size = DATA_DEFAULT_SIZE;
    
    if ((ret->data = create_segment( ret,"data", sec_address, sec_size, MEM_READ | MEM_WRITE , false)) == NULL )
        goto cleanup;

//bss_init:
    sec_address = opt->bss_start;
    sec_size = opt->bss_size;
    
    if ( sec_address == MEM_DEFAULT )
        sec_address = ret->data->end_mem_address;
    
    if ( sec_size == MEM_DEFAULT )
        sec_size = DATA_DEFAULT_SIZE;
    
    if ((ret->bss = create_segment( ret,"bss", sec_address, sec_size, MEM_READ | MEM_WRITE , false)) == NULL )
        goto cleanup;

//stack_init:
    sec_address = opt->stack_start;
    sec_size = opt->stack_size;
    
    if ( sec_address == MEM_DEFAULT ) {
        if ( ret->mode == MEM_32 )
            sec_address = STACK_DEFAULT_ADDRESS_32;
        else if ( ret->mode == MEM_64 )
            sec_address = STACK_DEFAULT_ADDRESS_64;
        else
            goto cleanup;
    }
    
    if ( sec_size == MEM_DEFAULT )
        sec_size = STACK_DEFAULT_SIZE;
    
    if ( sec_size == 0 )
        goto cleanup;
    
    if ((ret->stack = create_segment( ret,"stack", sec_address, sec_size, MEM_READ | MEM_WRITE , true)) == NULL )
        goto cleanup;


// INITIALIZE MEMORY MAP ARRAY

    ret->segment_map = calloc( ret->nsegments, sizeof(segment) );
    if (ret->segment_map == NULL)
        goto cleanup;
    
    ret->segment_map[0] = ret->text;
    ret->segment_map[1] = ret->rodata;
    ret->segment_map[2] = ret->data;
    ret->segment_map[3] = ret->bss;
    ret->segment_map[4] = ret->stack;
    
    qsort( ret->segment_map, ret->nsegments, sizeof(segment), segments_comparer );

// CHECK segment VALIDITY
    for ( size_t i = 1; i < ret->nsegments; i++ ) {
        temp = ret->segment_map[i -1];
        if ( temp->end_mem_address > (ret->segment_map[i])->start_mem_address ) {
            goto cleanup;
        }
        if ( ret->mode == MEM_32 && ((ret->segment_map[i])->end_mem_address > MEM_32_MAX_ADDRESS) ){
            goto cleanup;
        }
    }

    return ret;
    

cleanup:
    if ( ret != NULL ) {
        if ( ret->text )
            destroy_segment(ret->text);
        
        if ( ret->rodata )
            destroy_segment(ret->rodata);
        
        if ( ret->data )
            destroy_segment(ret->data);
        
        if ( ret->bss )
            destroy_segment(ret->bss);
        
        if ( ret->stack )
            destroy_segment(ret->stack);
        
        if ( ret->segment_map )
            free(ret->segment_map);
    }

    return NULL;
}

void destroy_memory_map (memory_map * mm) {
    for ( size_t i = 0; i < mm->nsegments; i++ )
        destroy_segment( mm->segment_map[i] );
    
    free(mm->segment_map);
}

/*
 * BASIC MEMORY ACCESS
*/
size_t write_to_memory ( memory_map * mm, memory_addr address, void * buffer, size_t length ) {
    segment sect = search_address_segment(mm, address);
    size_t offset ;

    if ( sect == NULL )
        return 0;
    
    if ( ! PERM_IS_WRITE(sect->perm) )
        return 0;
    
    offset = address - sect->start_mem_address;
    
    if ( offset + length > sect->size )
        return 0;
    // the next feature could be writing/reading to the next segment (if writable/readble)
    memcpy( sect->pointer + offset, buffer, length );
   
    if ( (sect->is_stack && ( offset + length < sect->top ) ) || offset + length > sect->top )
        sect->top = offset + length;
    
    return length;
    
}

size_t read_from_memory( memory_map * mm, memory_addr address, void * buffer, size_t length ){
    segment sect = search_address_segment(mm, address);
    size_t offset ;
    
    if ( sect == NULL )
        return 0;
    
    if ( ! PERM_IS_READ(sect->perm) )
        return 0;
    
    offset = address - sect->start_mem_address;
    
    if ( offset + length > sect->size )
        return 0;
    // the next feature could be writing/reading to the next segment (if writable/readble)
    memcpy(buffer, sect->pointer + offset, length );
    
    return length;
}



/*
 * Privileged controllers, they do not modify the top field of the sector
*/
static size_t offset_write_to_segment( segment sect, size_t offset, void * buffer, size_t length ){
    if ( offset + length > sect->size )
        return 0;

    memcpy( sect->pointer + offset, buffer, length );

    return length;
}
static size_t offset_read_from_segment( segment sect, size_t offset, void * buffer, size_t length ){
    if ( offset + length > sect->size )
        return 0;

    memcpy( buffer, sect->pointer + offset, length );

    return length;
}


static inline memory_addr offset_write_segment ( segment sect, size_t offset, void * buffer, size_t length ) {
    if ( sect == NULL )
        return MEM_FAILURE;
    
    if ( buffer == NULL && length == 0 )
        return sect->start_mem_address;
    
    if( offset_write_to_segment(sect, offset, buffer, length) != length )
        return MEM_FAILURE;

    return sect->start_mem_address + offset ;
}

static inline memory_addr offset_read_segment ( segment sect, size_t offset, void * buffer, size_t length ) {
    if ( sect == NULL )
        return MEM_FAILURE;
    
    if ( buffer == NULL && length == 0 )
        return sect->start_mem_address;
    
    if( offset_read_from_segment(sect, offset, buffer, length) != length )
        return MEM_FAILURE;
    return sect->start_mem_address + offset ;
}



memory_addr offset_read_text ( memory_map * mm, size_t offset, void * buffer, size_t length ){
    return offset_read_segment( mm->text , offset, buffer, length);
}

memory_addr offset_read_rodata ( memory_map * mm, size_t offset, void * buffer, size_t length ){
    return offset_read_segment( mm->rodata , offset, buffer, length);
}

memory_addr offset_read_data   ( memory_map * mm, size_t offset, void * buffer, size_t length ){
    return offset_read_segment( mm->data , offset, buffer, length);
}

memory_addr offset_read_bss    ( memory_map * mm, size_t offset, void * buffer, size_t length ){
    return offset_read_segment( mm->bss , offset, buffer, length);
}

memory_addr offset_read_stack    ( memory_map * mm, size_t offset, void * buffer, size_t length ){
    segment sect = mm->stack ;

    if ( sect == NULL )
        return 0;

    if ( buffer == NULL && length == 0 )
        return sect->end_mem_address;
    
    if ( offset > sect->size || offset < length )
        return 0;

    if( offset_read_from_segment(sect, sect->size - offset, buffer, length) != length )
        return MEM_FAILURE;
    
    return sect->end_mem_address - offset ; 
}



memory_addr offset_write_text ( memory_map * mm, size_t offset, void * buffer, size_t length ){
    return offset_write_segment( mm->text , offset, buffer, length);
}

memory_addr offset_write_rodata ( memory_map * mm, size_t offset, void * buffer, size_t length ){
    return offset_write_segment( mm->rodata , offset, buffer, length);
}

memory_addr offset_write_data   ( memory_map * mm, size_t offset, void * buffer, size_t length ){
    return offset_write_segment( mm->data , offset, buffer, length);
}

memory_addr offset_write_bss    ( memory_map * mm, size_t offset, void * buffer, size_t length ){
    return offset_write_segment( mm->bss , offset, buffer, length);
}

memory_addr offset_write_stack    ( memory_map * mm, size_t offset, void * buffer, size_t length ){
    segment sect = mm->stack ;

    if ( sect == NULL )
        return 0;

    if ( buffer == NULL && length == 0 )
        return sect->end_mem_address;
    
    if ( offset > sect->size || offset < length )
        return 0;

    if( offset_write_to_segment(sect, sect->size - offset, buffer, length) != length )
        return MEM_FAILURE;
    
    return sect->end_mem_address - offset ; 
}

/*
 * PRIVILEGED READ / WRITE
*/
size_t force_write_to_memory ( memory_map * mm, memory_addr address, void * buffer, size_t length ) {
    segment sect = search_address_segment(mm, address);
    size_t offset ;

    if ( sect == NULL )
        return 0;
    
    offset = address - sect->start_mem_address;
    return offset_write_to_segment(sect, offset, buffer, length);
}

size_t force_read_from_memory ( memory_map * mm, memory_addr address, void * buffer, size_t length ) {
    segment sect = search_address_segment(mm, address);
    size_t offset ;

    if ( sect == NULL )
        return 0;
    
    offset = address - sect->start_mem_address;
    
    return offset_read_from_segment(sect, offset, buffer, length);
}



/*
 * Stack-like functions for adding data easily
*/
static inline memory_addr push_to_segment( segment sect, void * buffer, size_t length ) {
    
    if (sect == NULL)
        return MEM_FAILURE;
    
    if ( buffer == NULL && length == 0 )
        return sect->start_mem_address + sect->top;
        
    if ( sect->top + length > sect->size )
        return MEM_FAILURE; 
                                     
    if( offset_write_to_segment(sect, sect->top, buffer, length) != length )
        return MEM_FAILURE;
        
    sect->top += length;
    
    return sect->start_mem_address + sect->top - length;
}


memory_addr push_text  ( memory_map * mm, void * buffer, size_t length ){
    return push_to_segment(mm->text, buffer, length);
}
memory_addr push_rodata  ( memory_map * mm, void * buffer, size_t length ){
    return push_to_segment(mm->rodata, buffer, length);
}
memory_addr push_data  ( memory_map * mm, void * buffer, size_t length ){
    return push_to_segment(mm->data, buffer, length);
}
memory_addr push_bss  ( memory_map * mm, void * buffer, size_t length ){
    return push_to_segment(mm->bss, buffer, length);
}

memory_addr push_stack ( memory_map * mm, void * buffer, size_t length ) {
    segment sect = mm->stack;
    if (sect == NULL)
        return MEM_FAILURE;
    
    if ( buffer == NULL && length == 0 )
        return sect->start_mem_address + sect->top;
    
    if ( sect->top < length )
        return MEM_FAILURE;    
                                                     
    if( offset_write_to_segment(sect, sect->top - length, buffer, length) != length )
        return MEM_FAILURE;
        
    sect->top -= length;
    return sect->start_mem_address + sect->top;
}

memory_addr pop_stack  ( memory_map * mm, void * buffer, size_t length ) {
    segment sect = mm->stack;
    
    if (sect == NULL)
        return MEM_FAILURE;
        
    if ( sect->top + length > sect->size )
        return MEM_FAILURE;
                              
    if( offset_read_from_segment(sect, sect->top, buffer, length) != length )
        return MEM_FAILURE;

    sect->top += length;
    return sect->start_mem_address + sect->top - length;
}

/*
 *
*/
void * get_real_memory_pointer( memory_map * mm, memory_addr address ) {
    segment sect = search_address_segment(mm, address);
    size_t offset ;
    
    if ( sect == NULL )
        return NULL;
    
    offset = address - sect->start_mem_address;

    return sect->pointer + offset;

}


