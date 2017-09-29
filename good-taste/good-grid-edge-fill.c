grid_edge_fill(grid)
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
