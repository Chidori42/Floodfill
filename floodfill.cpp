#include <iostream>
#include <vector>
#include <queue>

using namespace std;

// Player and target positions
int P_x = 1, P_y = 4;
int N_x, N_y;

// 2D grid representing the game map layout, where:
// '1' = wall, '0' = open path, 'P' = player start position, 'N' = target position
vector<vector<char>> grid = {
        { '1', '1', '1', '1', '1', '1' },
        { '1', '0', '0', '0', '1', '1' },
        { '1', '0', '1', '0', '1', '1' },
        { '1', '0', '0', '0', 'N', '1' },
        { '1', 'P', '1', '0', '0', '1' },
        { '1', '1', '1', '1', '1', '1' }
    };

// Recursive flood fill function that "marks" connected open cells and identifies the target's location
int flood_fill(vector<vector<char>>& grid, int x, int y)
{
    // Check bounds and exit if out of bounds or if the cell is a wall or target ('N')
    if (x < 0 || y < 0 || y >= grid.size() || x >= grid[0].size())
        return false;
    
    // Identify the position of the target
    if (grid[y][x] == 'N')
    {
        N_x = x;
        N_y = y;
    }

    // Stop recursion if the cell is a wall or the target
    if (grid[y][x] == '1' || grid[y][x] == 'N')
        return false;
    
    // Mark the current cell as visited by setting it to '1'
    grid[y][x] = '1';

    // Continue flood fill in all four directions (right, left, down, up)
    flood_fill(grid, x + 1, y);
    flood_fill(grid, x - 1, y);
    flood_fill(grid, x, y + 1);
    flood_fill(grid, x, y - 1);

    return false;
}

// Converts the grid layout to an integer grid where cells represent the shortest distance to the target
void convert_array(vector<vector<int>>& int_grid, int n_x, int n_y, int step)
{
    // Base case: if we've already visited this cell with a shorter or equal step value, or if it's the player position, exit
    if (int_grid[n_y][n_x] <= step && int_grid[n_y][n_x] != 0)
        return;
    if (P_x == n_x && P_y == n_y)
        return;

    // Update the distance at this position
    int_grid[n_y][n_x] = step;

    // Recursive calls to expand to adjacent cells with incremented step count
    convert_array(int_grid, n_x + 1, n_y, step + 1);
    convert_array(int_grid, n_x - 1, n_y, step + 1);
    convert_array(int_grid, n_x, n_y + 1, step + 1);
    convert_array(int_grid, n_x, n_y - 1, step + 1);
}

// BFS to find the shortest path from target to player using the distance grid
vector<pair<int, int>> find_shortest_path(vector<vector<int>>& int_grid, int n_x, int n_y, int p_x, int p_y)
{
    // Queue and visited set up for BFS traversal; parent tracks previous cell for path reconstruction
    queue<pair<int, int>> q;
    vector<vector<bool>> visited(int_grid.size(), vector<bool>(int_grid[0].size(), false));
    vector<vector<pair<int, int>>> parent(int_grid.size(), vector<pair<int, int>>(int_grid[0].size(), {-1, -1}));

    q.push({n_x, n_y});
    visited[n_y][n_x] = true;

    while (!q.empty())
    {
        int x = q.front().first;
        int y = q.front().second;
        q.pop();

        // If we reach the player, reconstruct and return the path
        if (x == p_x && y == p_y)
        {
            vector<pair<int, int>> path;
            while (x != n_x || y != n_y)
            {
                path.push_back({x, y});
                int px = parent[y][x].first;
                int py = parent[y][x].second;
                x = px;
                y = py;
            }
            path.push_back({n_x, n_y});
            reverse(path.begin(), path.end());
            return path;
        }

        // Explore neighbors (left, right, up, down) if they are unvisited and not walls
        if (x > 0 && !visited[y][x - 1] && int_grid[y][x - 1] != -1)
        {
            q.push({x - 1, y});
            visited[y][x - 1] = true;
            parent[y][x - 1] = {x, y};
        }
        if (x < int_grid[0].size() - 1 && !visited[y][x + 1] && int_grid[y][x + 1] != -1)
        {
            q.push({x + 1, y});
            visited[y][x + 1] = true;
            parent[y][x + 1] = {x, y};
        }
        if (y > 0 && !visited[y - 1][x] && int_grid[y - 1][x] != -1)
        {
            q.push({x, y - 1});
            visited[y - 1][x] = true;
            parent[y - 1][x] = {x, y};
        }
        if (y < int_grid.size() - 1 && !visited[y + 1][x] && int_grid[y + 1][x] != -1)
        {
            q.push({x, y + 1});
            visited[y + 1][x] = true;
            parent[y + 1][x] = {x, y};
        }
    }

    return {}; // No path found
}

int main()
{
    vector<vector<int>> int_grid(6, vector<int>(6, 0));
    
    // Convert the character grid into an integer grid where:
    // -1 represents walls, and the rest will be filled with distance values
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            if (grid[i][j] == '1')
                int_grid[i][j] = -1;
        }
    }

    // Perform flood fill starting from the player’s position to locate the target position
    flood_fill(grid, 1, 4);

    // Populate the integer grid with distances from the target using recursive filling
    convert_array(int_grid, N_x, N_y, 1);

    cout << "Integer grid with distances (0=Player, -1=wall):\n";
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            cout << int_grid[i][j] << "\t";
        }
        cout << endl;
    }

    // Compute and print the shortest path from the target to the player
    vector<pair<int, int>> shortest_path = find_shortest_path(int_grid, N_x, N_y, 1, 4);

    cout << "Shortest path from enemy to player:" << endl;
    for (const auto& cell : shortest_path)
    {
        cout << "(" << cell.first << ", " << cell.second << ")" << endl;
    }

    return 0;
}
