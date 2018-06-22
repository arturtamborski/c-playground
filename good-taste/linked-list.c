bad_remove_list_entry(entry)
{
    prev = NULL;
    walk = NULL;

    // walk the list 
    while (walk != entry)
    {
        prev = walk;
        walk = walk->next;
    }

    // remove the entry by updating the 
    // head or the previous entry
    if (!prev)
        head = entry->next;
    else
        prev->next = entry->next;
}


good_remove_list_entry(entry)
{
    // the "indirect" pointer points to the *address*
    // of the thing we'll update
    indirect = &head;

    // walk the list, looking for the thing that 
    // points to the entry we want to remove 
    while ((*indirect) != entry)
        indirect = &(*indirect)->next;

    // ... and just remove it by overwriting it
    *indirect = entry->next;
}
