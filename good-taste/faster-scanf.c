size_t scan_zu(size_t *ret)
{
	*ret = 0;
	int chr = getchar_unlocked();

	while (chr < 48 || chr > 57)
		chr = getchar_unlocked();

    	while (chr > 47 && chr < 58) {
        	*ret = (*ret << 1) + (*ret << 3) + (size_t)(chr - 48);
		chr = getchar_unlocked();
	}

	return *ret;
}
