
#include "memory.h"


#define MEM_32_MAX_ADDRESS 0xffffffff
#define MEM_64_MAX_ADDRESS 0xffffffffffffffff

#define MEM_MODE_IS_32(mm) (mm->mode == MEM_32)
#define MEM_MODE_IS_64(mm) (mm->mode == MEM_64)
#define IS_VALID_ADDRESS(mm, addr) ( addr <= ( (MEM_MODE_IS_32(mm) ) ? MEM_32_MAX_ADDRESS : ((MEM_MODE_IS_64(mm) ) ? MEM_64_MAX_ADDRESS : 0 ) ) )

#define MEM_SEGMENT_SET     5

#define PERM_IS_EXEC(perm) ((perm & MEM_EXEC) != 0)
#define PERM_IS_WRITE(perm) ((perm & MEM_WRITE) != 0)
#define PERM_IS_READ(perm) ((perm & MEM_READ) != 0)


typedef struct segment_struct{
    segment_id  id;

    memory_addr    start_mem_address;
    memory_addr    end_mem_address;
    
    size_t      size;
    size_t      top;
    void *      pointer;
    
    memory_perm perm;
    bool        is_stack;
} segment_struct;
typedef segment_struct * segment;



struct memory_map_struct {
    size_t  nsegments;
    memory_mode mode;
    
    size_t  map_size;
    segment * segment_map;
    segment * segment_id_table;
};


static segment create_segment( memory_map * mm, segment_id id, memory_addr start_address, size_t size, memory_perm perm, bool is_stack ) {
    segment new_segment = calloc( sizeof( segment_struct ) ,1 );
    
    if ( new_segment == NULL )
        goto cleanup;
    
    #define end_addr start_address + size
    
    if ( ! IS_VALID_ADDRESS(mm, end_addr) )
        goto cleanup;
    
    new_segment->id = id;
    
    if ( is_stack == true ) {
        
        if ( size > start_address )
            goto cleanup;
        new_segment->start_mem_address = start_address - size;
        new_segment->end_mem_address = start_address ;
        new_segment->top = size;
    }
    else{
    
        if ( mm->mode == MEM_64 && ( unsigned_add_overflow(start_address, size, sizeof(uint64_t), NULL) == true ||  start_address + size < start_address ) )
            goto cleanup;
            
        if ( mm->mode == MEM_32 && ( unsigned_add_overflow(start_address, size, sizeof(uint32_t), NULL) == true ||  start_address + size < start_address ) )
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

static segment search_segment_from_address ( memory_map * mm, const memory_addr addr ){
    segment * sect_ptr = (segment *)bsearch( &addr, mm->segment_map, mm->nsegments, sizeof(segment), address_in_segment );
    
    if ( sect_ptr == NULL )
        return NULL;
    
    return *sect_ptr;
}

static segment search_segment_from_id ( memory_map * mm, const segment_id id ) {
    segment seg = mm->segment_id_table[id];
    
    return seg;
}


static inline void sort_segment_map( memory_map * mm ){
    qsort( mm->segment_map, mm->nsegments, sizeof(segment), segments_comparer );
}

static void push_segment_into_segment_map(memory_map * mm, segment seg) {

    if ( mm->nsegments >= mm->map_size ) {
        mm->segment_map = realloc( mm->segment_map, mm->map_size + MEM_SEGMENT_SET );
        mm->map_size += MEM_SEGMENT_SET;
    }
    
    mm->segment_map[ mm->nsegments ] = seg;
    mm->nsegments++;
    return;
}

static bool check_boundary( memory_map * mm, memory_addr start, size_t size ) {

    if ( unsigned_add_overflow(start, size, sizeof(memory_addr), NULL ) == true )
        return false;

    #define end start + size
    for ( size_t i = 0; i < mm->nsegments; i++ ) {
        segment temp = mm->segment_map[i];
        
        if ( start >= temp->start_mem_address && start < temp->end_mem_address )
            return false;
        
        if ( end < temp->end_mem_address && end > temp->start_mem_address )
            return false;
    }
    #undef end
    return true;
}
//
//
//DEBUG
static void print_section(segment sec){
        printf("segment %u\n",      sec->id);
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

//
//
//

memory_map * create_memory_map( memory_mode mode ) {

    memory_map * ret = calloc(1, sizeof(memory_map) ) ;
    
    if ( ret == NULL )
        goto cleanup;
    
    ret->mode = mode;
    ret->nsegments = 0;
    ret->map_size = MEM_SEGMENT_SET;

    ret->segment_map = calloc( ret->map_size, sizeof(segment) );
    if (ret->segment_map == NULL)
        goto cleanup;
    
    ret->segment_id_table = calloc( MAX_SEGMENT_ID , sizeof(segment) );
    if ( ret->segment_id_table == NULL )
        goto cleanup;

    return ret;
    

cleanup:
    if ( ret != NULL ) {
        if ( ret->segment_map )
            free(ret->segment_map);
        
        if (ret->segment_id_table )
            free(ret->segment_id_table);
    }

    return NULL;
}

void destroy_memory_map (memory_map * mm) {
    for ( size_t i = 0; i < mm->nsegments; i++ )
        destroy_segment( mm->segment_map[i] );
    
    free(mm->segment_id_table);
    free(mm->segment_map);
    free(mm);
}


//
//
//




int add_segment_to_map ( memory_map * mm, segment_id newid, memory_addr start, size_t size, memory_perm perm, bool is_stack ) {

    size += (size % MEM_PAGE_SIZE );
    
    if ( newid >= MAX_SEGMENT_ID )
        goto cleanup;
    
    if ( check_boundary(mm, start, size) == false )
        goto cleanup;
    
    segment nseg = create_segment(mm, newid, start, size, perm, is_stack);
    if (nseg == NULL)
        goto cleanup;
    
    push_segment_into_segment_map(mm, nseg);
    sort_segment_map(mm);
    
    mm->segment_id_table[newid] = nseg;
    
    return 0;

cleanup:
    if (nseg)
        free(nseg);
    return -1;
}



/*
 * BASIC MEMORY ACCESS
*/
size_t write_to_memory ( memory_map * mm, memory_addr address, void * buffer, size_t length ) {
    segment sect = search_segment_from_address(mm, address);
    size_t offset ;

    if ( sect == NULL || buffer == NULL )
        return 0;
    
    if ( ! PERM_IS_WRITE(sect->perm) )
        return 0;
    
    offset = address - sect->start_mem_address;
    
    if ( offset + length > sect->size )
        return 0;
    
    memcpy( sect->pointer + offset, buffer, length );
    
    if ( (sect->is_stack && ( offset + length < sect->top ) ) || offset + length > sect->top )
        sect->top = offset + length;
    
    return length;
    
}

size_t read_from_memory( memory_map * mm, memory_addr address, void * buffer, size_t length ){
    segment sect = search_segment_from_address(mm, address);
    size_t offset ;
    
    if ( sect == NULL || buffer == NULL )
        return 0;
    
    if ( ! PERM_IS_READ(sect->perm) )
        return 0;
    
    offset = address - sect->start_mem_address;
    
    if ( offset + length > sect->size )
        return 0;
    
    memcpy(buffer, sect->pointer + offset, length );
    
    return length;
}


/*
 * Privileged controllers
*/
size_t force_write_to_memory ( memory_map * mm, memory_addr address, void * buffer, size_t length ) {
    segment sect = search_segment_from_address(mm, address);
    size_t offset ;

    if ( sect == NULL || buffer == NULL )
        return 0;
    
    offset = address - sect->start_mem_address;
    
    if ( offset + length > sect->size )
        return 0;

    memcpy( sect->pointer + offset, buffer, length );

    return length;
}

size_t force_read_from_memory ( memory_map * mm, memory_addr address, void * buffer, size_t length ) {
    segment sect = search_segment_from_address(mm, address);
    size_t offset ;

    if ( sect == NULL || buffer == NULL )
        return 0;
    
    offset = address - sect->start_mem_address;
    
    if ( offset + length > sect->size )
        return 0;
    
    memcpy(buffer, sect->pointer + offset, length );
    
    return length;
}


memory_addr offset_write_to_segment ( memory_map * mm, segment_id id, size_t offset, void * buffer, size_t length ) {
    segment sect = search_segment_from_id(mm, id);

    if ( sect == NULL )
        return MEM_FAILURE;
    
    if ( sect->is_stack == false ) {
        if ( buffer == NULL && length == 0 )        // get info about segment
            return sect->start_mem_address;
        
        if ( offset + length > sect->size )
            return MEM_FAILURE;
        
        memcpy(sect->pointer + offset, buffer, length );
        
        return sect->start_mem_address + offset ;
    }
    else {
        if ( buffer == NULL && length == 0 )
            return sect->end_mem_address;
        
        if ( offset > sect->size || offset < length )
            return MEM_FAILURE;
        
        if ( offset + length > sect->size )
            return MEM_FAILURE;
        
        memcpy(sect->size - offset, buffer, length);
        
        return sect->end_mem_address - offset ;
    }
}

memory_addr offset_read_from_segment ( memory_map * mm, segment_id id, size_t offset, void * buffer, size_t length ) {
    segment sect = search_segment_from_id(mm, id);

    if ( sect == NULL )
        return MEM_FAILURE;
    
    if ( sect->is_stack == false ) {
        if ( buffer == NULL && length == 0 )        // get info about segment
            return sect->start_mem_address;
        
        if ( offset + length > sect->size )
            return MEM_FAILURE;
        
        memcpy(buffer, sect->pointer + offset, length );
        
        return sect->start_mem_address + offset ;
    }
    else {
        if ( buffer == NULL && length == 0 )
            return sect->end_mem_address;
        
        if ( offset > sect->size || offset < length )
            return MEM_FAILURE;
        
        if ( offset + length > sect->size )
            return MEM_FAILURE;
        
        memcpy(buffer, sect->size - offset, length);
        
        return sect->end_mem_address - offset ;
    }
}

memory_addr push_to_segment  ( memory_map * mm, segment_id id, void * buffer, size_t length ) {
    segment sect = search_segment_from_id(mm, id);
    size_t offset;
    
    if (sect == NULL)
        return MEM_FAILURE;
    
    offset = sect->top;
    
    if ( sect->is_stack == false ) {
        if ( buffer == NULL && length == 0 )
            return sect->start_mem_address + sect->top;

        if ( offset + length > sect->size )
            return MEM_FAILURE;
        
        memcpy(sect->pointer + offset, buffer, length );
        sect->top += length;
        
        return sect->start_mem_address + offset;
    }
    else {
        if ( buffer == NULL && length == 0 )
            return sect->start_mem_address + sect->top;
        
        if ( offset < length )
            return MEM_FAILURE;    
                                                         
        if ( offset + length > sect->size )
            return MEM_FAILURE;
        
        memcpy(sect->size - offset, buffer, length);
        sect->top -= length;
        
        return sect->start_mem_address + offset - length;
    }
}

memory_addr pop_from_segment  ( memory_map * mm, segment_id id, void * buffer, size_t length ) {
    segment sect = search_segment_from_id(mm, id);
    size_t offset;
    
    if (sect == NULL)
        return MEM_FAILURE;
    
    offset = sect->top;
    
    if( sect->is_stack == false ){
        if ( buffer == NULL && length == 0 )
            return sect->start_mem_address + sect->top;
        
        if ( offset < length )
            return MEM_FAILURE;    
                                                         
        if ( offset + length > sect->size )
            return MEM_FAILURE;
        
        memcpy(sect->size - offset, buffer, length);
        sect->top -= length;
        
        return sect->start_mem_address + offset - length;
    }else{
        if ( buffer == NULL && length == 0 )
            return sect->start_mem_address + sect->top;
        if (sect == NULL)
            return MEM_FAILURE;
        if ( offset < length )
            return MEM_FAILURE;                        
        if ( offset + length > sect->size )
            return MEM_FAILURE;
        
        memcpy(sect->pointer + offset, buffer, length );
        sect->top += length;
        
        return sect->start_mem_address + offset;
    }
}


void * get_real_memory_pointer( memory_map * mm, memory_addr address ) {
    segment sect = search_segment_from_address(mm, address);
    size_t offset ;
    
    if ( sect == NULL )
        return NULL;
    
    offset = address - sect->start_mem_address;

    return sect->pointer + offset;

}

bool check_address_perm(memory_map * mm, memory_addr addr, memory_perm perm ) {
    segment sect = search_segment_from_address(mm, addr);
    if (sect == NULL)
        return false;
    
    if ( perm & sect->perm == 0 )
        return false;
    
    return true;
}



int iter_memory_map ( memory_map * mm, bool (* iter) ( segment_id id, memory_addr start, size_t size, bool is_stack, void * extra ), void * extra ) {
    if ( mm == NULL  || iter == NULL)
        return -1;
    
    segment seg;
    
    for( size_t i = 0; i < mm->nsegments; i++ ){
        seg = mm->segment_map[i];
        
        if ( (iter)( seg->id, seg->start_mem_address, seg->size, seg->is_stack, extra ) == false )
            break;
    }
    
    return 0;
}



