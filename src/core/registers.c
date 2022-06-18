
#include "registers.h"


typedef struct register_struct {
    register_size   size;   // byte size of the register
    void *          address;     // register's memory address
    register_id     parent;
    bool            is_partition;
} register_struct ;


struct register_list_struct {
    register_id     nregs;
    register_struct * regs_list;
};

static bool is_in_use( register_struct * reg ){
    if( reg->size != 0 )
        return true;
    
    return false;
}

static int delete_partition( register_struct * part ) {
    part->size = 0;
    part->address = NULL;
    part->parent = 0;
    part->is_partition = false;
    
    return 0;
}

register_list * create_register_list( register_id size ){

    if ( size == 0 )
        goto cleanup;

    register_list * rl = NULL;
    
    rl = calloc( 1, sizeof(register_list) );
    if ( rl == NULL )
        goto cleanup;
    
    rl->nregs = size;
    rl->regs_list = calloc(size, sizeof(register_struct) );
    if ( rl->regs_list == NULL )
        goto cleanup;
    
    return rl;

cleanup:
    if ( rl != NULL ) {
        if ( rl->regs_list != NULL )
            free(rl->regs_list);
        
        free(rl);
    }
    
    return NULL;
}

void destroy_register_list( register_list * regl) {
    if ( regl == NULL )
        return;
    
    register_struct * node;
    
    for ( size_t i = 0; i < regl->nregs; i++ ) {
        node = &regl->regs_list[ i ];
        if ( is_in_use(node) == true && node->is_partition == false  )
            free(node->address);
    }
    
    free(regl->regs_list);
    free(regl);
}



int add_register_to_list    ( register_list * regl, register_id regid, register_size size ) {
    if ( regl == NULL || regid >= regl->nregs)
        return -1;
    
    if ( is_in_use( &regl->regs_list[ regid ] ) == true )
        return -1;
    
    register_struct target;
    target.size = size;
    target.address = calloc(1, size);
    target.parent = 0;
    target.is_partition = false;

    regl->regs_list[regid] = target;
    
    return 0;
}

int add_register_partition_to_list ( register_list * regl, register_id regid, register_id parentid, register_size offset, register_size size ) {
    if ( regl == NULL || regid >= regl->nregs || parentid >= regl->nregs )
        return -1;
    
    if ( is_in_use( &regl->regs_list[ regid ] ) == true )
        return -1;
    
    register_struct * parent = &regl->regs_list[ parentid ];
    if ( is_in_use(parent) == false )
        return -1;
    
    if( offset >= parent->size )
        return -1;
    
    if ( size > parent->size )
        return -1;
    
    if ( offset >= parent->size - size  )
        return -1;
    
    register_struct target;
    
    target.size = size;
    target.address = ((uint8_t *)parent->address) + offset;
    target.parent = parentid;
    target.is_partition = true;
    regl->regs_list[regid] = target;
    
    return 0;
}

int del_register_from_list  ( register_list * regl, register_id regid ){
    if ( regl == NULL || regid >= regl->nregs)
        return -1;
    
    register_struct * target = &regl->regs_list[regid];
    register_struct * node;

    if ( target->is_partition == true ) {
        delete_partition(target);
        return 0;
    }
    
    for ( size_t i = 0; i < regl->nregs; i++ ) {
        node = &regl->regs_list[ i ];
        if ( is_in_use(node) == true && node->is_partition == true && node->parent == regid )
            delete_partition(node);
    }
    
    target->size = 0;
    free(target->address);
    target->address = NULL;
    
    return 0;
}

/*
 * @regl    register list from where the register will be written/readden
 * @regid   ID number of the register
 * @buffer  buffer where the register's data have to be stored or where to read the new data from,
 *          the size of the buffer must be at least equal to the size of the register
 *
 * @return  number of bytes readden / written
*/
register_size read_from_register  ( register_list * regl, register_id regid, void * buffer) {
    if ( regl == NULL || regid >= regl->nregs || buffer == NULL )
        goto cleanup;
    
    register_struct target = regl->regs_list[regid];
    
    if ( target.size == 0 || target.address == NULL )
        goto cleanup;
    
    memcpy( buffer, target.address, target.size );
    
    return target.size;

cleanup:
    return 0;
}

register_size write_into_register ( register_list * regl, register_id regid, void * buffer) {
    if ( regl == NULL || regid >= regl->nregs || buffer == NULL )
        goto cleanup;
    
    register_struct target = regl->regs_list[regid];
    
    if ( target.size == 0 || target.address == NULL )
        goto cleanup;
    
    memcpy( target.address, buffer, target.size );
    
    return target.size;

cleanup:
    return 0;
}


int iter_register_list ( register_list * regl, bool (* iter) ( register_id id, register_size, bool is_partition, void * extra ), void * extra ) {
    for ( size_t i = 0; i < regl->nregs; i++  )
    {
        register_struct * node = &regl->regs_list[i];
        
        if ( !is_in_use(node) )
            continue;
        
        if ( (iter)( i, node->size, node->is_partition, extra ) == false )
            break;
    }
    return 0;
}


