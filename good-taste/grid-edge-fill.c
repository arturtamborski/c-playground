bad_grid_edge_fill(grid)
{
    for (r = 0; r < GRID_SIZE; ++r)
    {
        for (c = 0; c < GRID_SIZE; ++c)
        {
            // top
            if (r == 0)
                grid[r][c] = 1;

            // left
            if (c == 0)
                grid[r][c] = 1;

            // right
            if (c == GRID_SIZE - 1)
                grid[r][c] = 1;
        
            // right
            if (r == GRID_SIZE - 1)
                grid[r][c] = 1;
        }
    }
}


good_grid_edge_fill(grid)
{
    for (i = 0; i < GRID_SIZE; i++)
    {
        // top
        grid[0][i] = 1;

        // bottom
        grid[GRID_SIZE - 1][i] = 1;

        // left
        grid[i][0] = 1;
        
        // right
        grid[i][GRID_SIZE - 1] = 1;
    }
}
