remove_list_entry(entry)
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
