#include <iostream>
#include <vector>
#include <fstream> //for table lookup/write in table files
#include <sstream> //for converting between int and string
#include <sys/stat.h> //for checking if table files exist
#include <ctime> //for timing computation
#include "tree.h" //custom classes, structs
#include <algorithm> //for using find on vectors

using namespace std;

void print_node_list(vector<Node> nodeList);
int lookup(string filename, int x, int y);
bool write(string filename, int x, int y, int val);
bool get_int_param(string inputStr, int& result);
bool out_of_bounds(int row, int col);
bool file_exists(string filename);
bool initialize_file(string filename);
bool move_separates(vector<vector<bool>> &grid, int row, int col, int x, int y, int prevX, int prevY);
void print_grid (vector<vector<bool>> &grid, int row, int col);

/// Maximum grid dimensions
/// MAX_DIM_K = max allowed grid dim when one dim is K
int MAX_DIM = 10;
int MAX_DIM_2 = 100;
int MAX_DIM_3 = 23;
int MAX_DIM_4 = 18;

/// What kinds of paths to find/count
/// 1: All paths
/// 2: Paths connecting opposite corners
/// 3: Paths connecting adjacent corners
/// 4: Count all three kinds above, but for just the values
///    ROW and COL below
///
/// Note: just leave this on 4 so it computes
/// one value of (ROW, COL) at a time.
int MODE = 4;



int main()
{
    ///
    ///  Getting parameters for computation
    ///

    // User selection of row/col to compute
    string inputStr;
    int ROW, COL;
    int defaultDim = 4;
    bool getNewParams = true;

    while (getNewParams) {
        ROW = COL = -1;

        // User inputs row/col dimensions
        cout << "Enter row: ";
        while (ROW < 0) {
            getline(cin, inputStr);
            if (!get_int_param(inputStr, ROW)) {
                ROW = defaultDim;
                cout << "Could not process input; row set to: " << ROW << endl;
            }
        }
        cout << "Enter column: ";
        while (COL < 0) {
            getline(cin, inputStr);
            if (!get_int_param(inputStr, COL)) {
                COL = defaultDim;
                cout << "Could not process input; column set to: " << COL << endl;
            }
        }

        // Check that dimensions are within bounds
        if (out_of_bounds(ROW, COL)) {
            ROW = defaultDim;
            COL = defaultDim;
            cout << "Dimensions are out of bounds." << endl;
        }
        cout << "Row set to: " << ROW << "\nColumn set to: " << COL << endl;

        // This will store the counts of each type of path
        PathCounts pathCounts = {-1, -1, -1};


        ///
        ///  Computing and counting paths
        ///

        // Computation counter and timer
        int loopCounter = 0;
        int millCounter = 0;
        clock_t start;

        // Loop over row/column values within range
        for (int row = 2; row <= MAX_DIM; row++) {
            int colStart;
            switch (MODE) {
                case 1:
                    colStart = row;
                break;
                case 2:
                    colStart = row;
                break;
                case 3:
                    colStart = 2;
                break;
                case 4:
                    if (ROW > MAX_DIM) { MAX_DIM = ROW; }
                    if (COL > MAX_DIM) { MAX_DIM = COL; }
                    row = ROW;
                    colStart = COL;
                break;
                default:
                    throw (MODE);
                break;
            }
        for (int col = colStart; col <= MAX_DIM; col++) {

            // Set up grid, position, node list
            vector<vector<bool>> grid;
            for (int i = 0; i < row; i++) {
                vector<bool> newRow;
                grid.push_back(newRow);
                for (int j = 0; j < col; j++) {
                    grid[i].push_back(false);
                }
            }
            grid[0][0] = true;
            int pathLength = row*col - 1;
            int pos[2] = {0,0};
            int pathCounter = 0;
            int oppPathCounter = 0;
            int adjPathCounter = 0;
            vector<Node> nodeList;
            nodeList.push_back(Node());

            // Set up vars for tree/path building loops
            bool openNode = true;
            int branchNodeInd = 0;
            int depth = 0;
            bool validMove = true;
            int currentNodeInd = branchNodeInd;
            Node currentNode = nodeList[currentNodeInd];
            bool moves[4] = {false, false, false, false};
            int children[4] = {-1, -1, -1, -1};
            int currentParentInd;
            Node currentParent;
            int currentType;

            // Set clock start
            start = clock();

            // Loop to build tree/find paths
            while (openNode) {
                openNode = false;
                nodeList[currentNodeInd].flip_open();

                // Loop to descend current branch
                while (validMove) {
                    validMove = false;
                    moves[0] = moves[1] = moves[2] = moves[3] = false;

                    // Detect valid moves and add nodes
                    //   Note: conditions check that move is in bounds, open, and does not
                    //   separate grid
                    if (   pos[1] < col - 1  //right
                        && !grid[pos[0]][pos[1]+1]
                        && !move_separates(grid, row, col, pos[0], pos[1]+1, pos[0], pos[1]) )
                    {
                        moves[0] = true;
                        validMove = true;
                        nodeList.push_back(
                            Node(true, 0, pos[0], pos[1]+1, currentNodeInd, -1, -1, -1, -1)
                        );
                        nodeList[currentNodeInd].set_right(nodeList.size()-1);
                    }
                    if (   pos[0] > 0  //up
                        && !grid[pos[0]-1][pos[1]]
                        && !move_separates(grid, row, col, pos[0]-1, pos[1], pos[0], pos[1]) )
                    {
                        moves[1] = true;
                        validMove = true;
                        nodeList.push_back(
                            Node(true, 1, pos[0]-1, pos[1], currentNodeInd, -1, -1, -1, -1)
                        );
                        nodeList[currentNodeInd].set_up(nodeList.size()-1);
                    }
                    if (   pos[1] > 0  //left
                        && !grid[pos[0]][pos[1]-1]
                        && !move_separates(grid, row, col, pos[0], pos[1]-1, pos[0], pos[1]) )
                    {
                        moves[2] = true;
                        validMove = true;
                        nodeList.push_back(
                            Node(true, 2, pos[0], pos[1]-1, currentNodeInd, -1, -1, -1, -1)
                        );
                        nodeList[currentNodeInd].set_left(nodeList.size()-1);
                    }
                    if (   pos[0] < row - 1  //down
                        && !grid[pos[0]+1][pos[1]]
                        && !move_separates(grid, row, col, pos[0]+1, pos[1], pos[0], pos[1]) )
                    {
                        moves[3] = true;
                        validMove = true;
                        nodeList.push_back(
                            Node(true, 3, pos[0]+1, pos[1], currentNodeInd, -1, -1, -1, -1)
                        );
                        nodeList[currentNodeInd].set_down(nodeList.size()-1);
                    }

                    // If child nodes were made above, select one and move to it
                    if (validMove) {
                        if (moves[0]) { currentNodeInd = nodeList[currentNodeInd].get_right(); }
                        else if (moves[1]) { currentNodeInd = nodeList[currentNodeInd].get_up(); }
                        else if (moves[2]) { currentNodeInd = nodeList[currentNodeInd].get_left(); }
                        else if (moves[3]) { currentNodeInd = nodeList[currentNodeInd].get_down(); }
                        else { throw moves; }
                        nodeList[currentNodeInd].flip_open();
                        pos[0] = nodeList[currentNodeInd].get_x();
                        pos[1] = nodeList[currentNodeInd].get_y();
                        grid[pos[0]][pos[1]] = true;
                        depth++;
                    }
                } // end descent loop

                // Inc path counter if full path was found, set vars for loop
                if (depth == pathLength) {
                    switch (MODE) {
                        case 1: // count any path
                            pathCounter++;
                        break;
                        case 2: // count opposite paths
                            if (    nodeList[currentNodeInd].get_x() == row - 1
                                &&  nodeList[currentNodeInd].get_y() == col - 1) {
                                pathCounter++;
                            }
                        break;
                        case 3: // count adjacent paths
                            if (    nodeList[currentNodeInd].get_x() == row - 1
                                &&  nodeList[currentNodeInd].get_y() == 0) {
                                pathCounter++;
                            }
                        break;
                        case 4: // count each kind with separate counters
                            pathCounter++;
                            if (    nodeList[currentNodeInd].get_x() == row - 1
                                &&  nodeList[currentNodeInd].get_y() == col - 1) {
                                oppPathCounter++;
                            } else if (    nodeList[currentNodeInd].get_x() == row - 1
                                &&  nodeList[currentNodeInd].get_y() == 0) {
                                adjPathCounter++;
                            }
                        break;
                        default:
                            throw (MODE);
                        break;
                    }
                }
                currentType = nodeList[currentNodeInd].get_type();
                vector<int> deleteInds;

                // Move back up the tree until open node found
                while (!openNode && currentType > -1) {
                    grid[pos[0]][pos[1]] = false;
                    currentParentInd = nodeList[currentNodeInd].get_parent();
                    currentParent = nodeList[currentParentInd];
                    deleteInds.push_back(currentNodeInd);

                    children[0] = currentParent.get_right();
                    children[1] = currentParent.get_up();
                    children[2] = currentParent.get_left();
                    children[3] = currentParent.get_down();

                    if (children[0] > -1 && nodeList[children[0]].is_open()) {
                        openNode = true;
                        branchNodeInd = currentParent.get_right();
                    } else if (children[1] > -1 && nodeList[children[1]].is_open()) {
                        openNode = true;
                        branchNodeInd = currentParent.get_up();
                    } else if (children[2] > -1 && nodeList[children[2]].is_open()) {
                        openNode = true;
                        branchNodeInd = currentParent.get_left();
                    } else if (children[3] > -1 && nodeList[children[3]].is_open()) {
                        openNode = true;
                        branchNodeInd = currentParent.get_down();
                    } else {
                        currentNodeInd = currentParentInd;
                        currentNode = nodeList[currentNodeInd];
                        currentType = currentNode.get_type();
                        pos[0] = currentNode.get_x();
                        pos[1] = currentNode.get_y();
                        depth--;
                    }
                }

                if (currentType > -1) { // if currentNode != root, i.e. more branches to do
                    // Delete nodes along ascended branch, update indices accordingly
                    branchNodeInd--;
                    int delChild = deleteInds.back();
                    nodeList[currentParentInd].set_right(children[0] - 1);
                    nodeList[currentParentInd].set_up(children[1] - 1);
                    nodeList[currentParentInd].set_left(children[2] - 1);
                    nodeList[currentParentInd].set_down(children[3] - 1);
                    if (children[0] == delChild) {
                        nodeList[currentParentInd].set_right(-1);
                    }
                    if (children[1] == delChild) {
                        nodeList[currentParentInd].set_up(-1);
                    }
                    if (children[2] == delChild) {
                        nodeList[currentParentInd].set_left(-1);
                    }
                    if (children[3] == delChild) {
                        nodeList[currentParentInd].set_down(-1);
                    }
                    for (int i = deleteInds.size()-1; i > -1; i--) {
                        nodeList.erase(nodeList.begin() + deleteInds[i]);
                    }
                    // If erased all but root, then all branches have been traversed
                    if (nodeList.size() == 1) { break; }

                    currentNodeInd = branchNodeInd;
                    currentNode = nodeList[currentNodeInd];
                    pos[0] = currentNode.get_x();
                    pos[1] = currentNode.get_y();
                    grid[pos[0]][pos[1]] = true;
                    validMove = true;
                }

                // Show loop count and time
                if (++loopCounter % 1000000 == 0) {
                    cout << "Computation time after "
                         << ++millCounter << " million loops: "
                         << ( (clock() - start) / double(CLOCKS_PER_SEC) ) << " seconds" << endl;
                    loopCounter = 0;
                }

            } // end tree build/path loop

            if (MODE == 4) {
                pathCounts.total = pathCounter;
                pathCounts.adj = adjPathCounter;
                pathCounts.opp = oppPathCounter;
                break;
            } else {
                cout << "Row: " << row
                    << " | Col: " << col
                    << " | Num paths: " << pathCounter
                    << endl;
            }

        } // end col values loop
        if (MODE == 4) { break; }
        } // end row/col values loop

        // End program if MODE != 4
        if (MODE != 4) { return 0; }

        // Print path counts and computation time
        cout << "Computation time: "
             << ( (clock() - start) / double(CLOCKS_PER_SEC) ) << " seconds"
             << "\nComputation loops: " << millCounter << " million and " << loopCounter
             << endl;

        cout << "Row: " << ROW << "\n"
             << " | Col: " << COL
             << " | Total paths: " << pathCounts.total
             << " | Opposite paths: " << pathCounts.opp
             << " | Adjacent paths: " << pathCounts.adj
             << endl;


        ///
        ///  Storing computed values in table files, asking to repeat computation
        ///

        // Check if tables exist; if not, initialize them with a 100x100 array of -1's
        string totalPath = "tables/Total Table.txt";
        string adjPath = "tables/Adjacent Table.txt";
        string oppPath = "tables/Opposite Table.txt";
        if (!file_exists(totalPath)) { initialize_file(totalPath); }
        if (!file_exists(adjPath)) { initialize_file(adjPath); }
        if (!file_exists(oppPath)) { initialize_file(oppPath); }

        // Write computed values to tables
        // Note: use ROW-2 and COL-2 because parameter values start at 2;
        //      i.e. we ignore the case when ROW==1 or COL==1 because the problem is trivial
        if (write(totalPath, ROW-2, COL-2, pathCounts.total)) {
            cout << "Successful write to total table" << endl;
        }
        if (write(adjPath, ROW-2, COL-2, pathCounts.adj)) {
            cout << "Successful write to adjacent table" << endl;
        }
        if (write(oppPath, ROW-2, COL-2, pathCounts.opp)) {
            cout << "Successful write to opposite table" << endl;
        }

        // Ask user if should repeat computation
        cout << "Compute path counts for new row/column values? ";
        getline(cin, inputStr);
        if (!inputStr.empty()) {
            if (inputStr[0] == 'y' || inputStr[0] == 'Y') {
                getNewParams = true;
            } else {
                getNewParams = false;
            }
        } else {
            getNewParams = false;
        }

    } // end getting params from user input loop

} // end main




/// Print nodeList, showing tree structure
void print_node_list(vector<Node> nodeList) {
    Node node;
    for (int i = 0; i < nodeList.size(); i++) {
        node = nodeList[i];
        cout << "Index: " << i
            << " | Type: " << node.get_type()
            << " | Parent: " << node.get_parent()
            << endl;
    }
}


/// Return int at table coord (x,y) in given file
int lookup(string filename, int x, int y) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "Could not open " << filename << endl;
        return 1;
    }

    string row;

    // Move down to x-th row
    for (int i = 0; i < x; i++) { getline(fin, row); }

    // Find y-th entry (comma delimiters)
    int commaCounter = 0;
    int num;
    char part;
    while (commaCounter < y + 1) {
        // Read to next comma, increment commaCounter
        fin >> num;
        fin >> part;
        if (part == ',') { commaCounter++; }
        else if (part == '\n') { cout << "Reached row end" << endl; break; }
        else {
            cout << "Read unexpected character: " << part << endl;
            fin.close();
            return -1;
        }
    }

    fin.close();
    return num;
}


/// Write val to table at coord (x,y) in given file
bool write(string filename, int x, int y, int val) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "Could not open " << filename << endl;
        return 1;
    }

    string row;
    vector<string> rows;

    // Store rows before x-th row
    for (int i = 0; i < x; i++) {
        getline(fin, row);
        rows.push_back(row);
    }

    // Split x-th row into entries
    getline(fin, row);
    vector<string> entries;
    int entryStart = 0;
    int entryEnd = 0;
    while (entryEnd < row.length()) {
        entryEnd = row.find(',', entryStart);
        if (entryEnd < 0) { entryEnd = row.length(); }
        entries.push_back(row.substr(entryStart, entryEnd - entryStart));
        entryStart = entryEnd + 1;
    }

    // Replace y-th entry by val
    stringstream ss;
    ss << val;
    entries[y] = ss.str();

    // Reconstruct row and store
    row = "";
    for (int i = 0; i < entries.size() - 1; i++) {
        row.append(entries[i]);
        row.append(",");
    }
    row.append(entries[entries.size()-1]);
    rows.push_back(row);

    // Store rows after x-th row
    while (getline(fin, row)) { rows.push_back(row); }

    // Print all rows to file, overwriting previous file
    ofstream fout(filename);
    if (!fout.is_open()) {
        cerr << "Could not open " << filename << endl;
        return false;
    }

    for (int i = 0; i < rows.size(); i++) { fout << rows[i] << endl; }
    fout.close();
    return true;
}


/// Process string of user input, attempting to extract int
bool get_int_param(string inputStr, int& result) {
    int x;
    stringstream ss(inputStr);
    while (!ss.eof()) {
        if (ss >> x) {
           result = x;
           return true;
        }
        ss.clear();
        ss.ignore();
    }
    return false;
}


/// Check if dimension is within bounds
bool out_of_bounds(int row, int col) {
    switch (row) {
        case 4: return (col > MAX_DIM_4);
        case 3: return (col > MAX_DIM_3);
        case 2: return (col > MAX_DIM_2);
        default:
            if (row < 2) { return true; }
            switch (col) {
                case 4: return (row > MAX_DIM_4);
                case 3: return (row > MAX_DIM_3);
                case 2: return (row > MAX_DIM_2);
                default:
                    if (col < 2) { return true; }
                    return (row > MAX_DIM || col > MAX_DIM);
            }
    }
}


/// Check if file exists
bool file_exists(string filename) {
    struct stat statBuf;
    return ( stat(filename.c_str(), &statBuf) == 0 );
}


/// Initialize file with 100x100 array of -1's
bool initialize_file(string filename) {
    cout << "Initializing file " << filename << endl;

    ofstream fout(filename);
    if (!fout.is_open()) {
        cerr << "Could not open " << filename << endl;
        return false;
    }

    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 99; j++) {
            fout << "-1,";
        }
        fout << "-1" << endl;
    }

    fout.close();
    return true;
}


/// Return true iff move separates graph
bool move_separates(vector<vector<bool>> &grid, int row, int col, int x, int y, int prevX, int prevY) {
    ///return false;
    // Special case: when row or col is 2, can use a simpler strategy
    if (row == 2) {
        return (   x != prevX
                && y > 0
                && y < col-1
                && !grid[x][y-1]
                && !grid[x][y+1] );
    }
    if (col == 2) {
        return (   y != prevY
                && x > 0
                && x < row-1
                && !grid[x-1][y]
                && !grid[x+1][y] );
    }

    // If previous move was on an edge, return false
    if (prevX == 0 || prevX == row-1 || prevY == 0 || prevY == col-1) { return false; }

    // Eliminate non-edge moves and select target positions
    //   Note: corner moves are already gone, because you cannot move from a non-edge square
    //   to a corner
    Position posA;
    Position posB;
    if (x == 0 || x == row-1) { //left or right edge
        posA.x = x;
        posA.y = y-1;
        posB.x = x;
        posB.y = y+1;
    } else if (y == 0 || y == col-1) { //top or bottom edge
        posA.x = x-1;
        posA.y = y;
        posB.x = x+1;
        posB.y = y;
    } else { //not an edge move, so return false
        return false;
    }

    // Check that posA, posB are open
    if (grid[posA.x][posA.y] || grid[posB.x][posB.y]) { return false; }

    // Build the component of posA and see if it contains posB
    //   Note: cannot make vector of arrays, so using structs instead.
    vector<Position> compA;
    compA.push_back(posA);
    Position posC;
    int firstOpenInd = 0;
    bool foundNew = true;

    // Component building loop
    while (foundNew) {
        foundNew = false;
        posA = compA[firstOpenInd];

        if (posA.x < row-1) {
            // Set posC to position one down from posA
            posC.x = posA.x + 1;
            posC.y = posA.y;
            if (!grid[posC.x][posC.y]) { //if posC is open
                if (posC == posB) { return false; } //return false if posB is found in component of A
                if (find(compA.begin(), compA.end(), posC) != compA.end()) { //if posC not already in compA
                    compA.push_back(posC);
                    if (!foundNew) {
                        firstOpenInd = compA.size()-1;
                        foundNew = true;
                    }
                }
            }
        }
        if (posA.x > 0) {
            // Set posC to position one up from posA
            posC.x = posA.x - 1;
            posC.y = posA.y;
            if (!grid[posC.x][posC.y]) {
                if (posC == posB) { return false; }
                if (find(compA.begin(), compA.end(), posC) != compA.end()) {
                    compA.push_back(posC);
                    if (!foundNew) {
                        firstOpenInd = compA.size()-1;
                        foundNew = true;
                    }
                }
            }
        }
        if (posA.y < col-1) {
            // Set posC to position one right from posA
            posC.x = posA.x;
            posC.y = posA.y + 1;
            if (!grid[posC.x][posC.y]) {
                if (posC == posB) { return false; }
                if (find(compA.begin(), compA.end(), posC) != compA.end()) {
                    compA.push_back(posC);
                    if (!foundNew) {
                        firstOpenInd = compA.size()-1;
                        foundNew = true;
                    }
                }
            }
        }
        if (posA.y > 0) {
            // Set posC to position one left from posA
            posC.x = posA.x;
            posC.y = posA.y - 1;
            if (!grid[posC.x][posC.y]) {
                if (posC == posB) { return false; }
                if (find(compA.begin(), compA.end(), posC) != compA.end()) {
                    compA.push_back(posC);
                    if (!foundNew) {
                        firstOpenInd = compA.size()-1;
                        foundNew = true;
                    }
                }
            }
        }
    } //end comp building loop

    // If we get here then posB was not found in the component of A, hence
    //   they are in separate components so the move separates graph
    ///cout << "separation caused by candidate move (" << x << "," << y << ")" << endl;
    return true;
}


/// Print grid
void print_grid (vector<vector<bool>> &grid, int row, int col) {
    cout << row << "x" << col << " grid: \n";
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            cout << grid[i][j] << " ";
        }
        cout << endl;
    }
}
