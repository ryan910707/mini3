#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include<bits/stdc++.h>
using namespace std;

const int SIZE = 8;


struct Point {
    int x, y;
	Point() : Point(0, 0) {}
	Point(float x, float y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};
//copy constructor
class OthelloBoard {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;
    int winner;
private:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
public:
    OthelloBoard(){

    }
    OthelloBoard(const OthelloBoard &copy){
        for(int i=0;i<SIZE;i++)for(int j=0;j<SIZE;j++) board[i][j]=copy.board[i][j];
        for(auto i:copy.next_valid_spots) next_valid_spots.push_back(i);
        for(int i=0;i<3;i++) disc_count[i]=copy.disc_count[i];
        cur_player=copy.cur_player;
    }
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
        if(!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) {
                // Game ends
                done = true;
                int white_discs = disc_count[WHITE];
                int black_discs = disc_count[BLACK];
                if (white_discs == black_discs) winner = EMPTY;
                else if (black_discs > white_discs) winner = BLACK;
                else winner = WHITE;
            }
        }
        return true;
    }
};

OthelloBoard first_board;
int value_arr[8][8]={{99, -8, 8,  6,  6,  8,  -8, 99 },
		    {-8, -24,  -4, -3, -3, -4, -24,  -8 }, 
		    {8, -4, 7,  4,  4,  7,  -4, 8 },
		    {6,  -3, 4,  0,  0,  4,  -3, 6  },
		    {6,  -3, 4,  0,  0,  4,  -3, 6 },
		    {8,  -4, 7,  4,  4,  7,  -4, 8 },
		    {-8, -24,  -4, -3, -3, -4, -24,  -8}, 
		    {99, -8, 8,  6,  6,  8,  -8, 99 }};

int state_value_function(OthelloBoard cur){
    int value=0;
    std::vector<Point> tmp = cur.get_valid_spots();
    for(auto i:tmp){
        value-=value_arr[i.x][i.y];
    }
    return value;
}


int alphabeta(OthelloBoard cur_board, int depth, int A, int B, int maximizingPlayer ){
    if (depth==0 || cur_board.next_valid_spots.empty()){
        return state_value_function(cur_board);
    }
    if( first_board.cur_player == maximizingPlayer){
        int value= INT_MIN;
        for(auto i:cur_board.next_valid_spots){
            OthelloBoard next_board(cur_board);
            next_board.put_disc(i);
            value= max(value, alphabeta(next_board, depth-1,A,B, 3-maximizingPlayer));
            A= max(A,value);
            if(A>=B) break;
        }
        return value;
    }
    else {
        int value=INT_MAX;
        for(auto i:cur_board.next_valid_spots){
            OthelloBoard next_board(cur_board);
            next_board.put_disc(i);
            value=min(value, alphabeta(next_board,depth-1,A,B,3-maximizingPlayer));
            B = min(B,value);
            if(A>=B) break;
        }
        return value;
    }
}



void read_board(std::ifstream& fin) {
    fin >> first_board.cur_player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> first_board.board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    float x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        first_board.next_valid_spots.push_back({x, y});
    }
}

void write_valid_spot(std::ofstream& fout) {
    int n_valid_spots = first_board.next_valid_spots.size();
    srand(time(NULL));
    Point p;
    int A=INT_MIN;
    int B=INT_MAX;
    int num=-1;
    for(int i=0;i<n_valid_spots;i++){
        OthelloBoard tmp(first_board);
        tmp.put_disc(first_board.next_valid_spots[i]);
        int cnt = alphabeta(tmp, 2, A, B, 3-first_board.cur_player);
        if(cnt > A ) {
            A=cnt, num = i;
            p=first_board.next_valid_spots[num];
            fout << p.x << " " << p.y << std::endl;
        }
    }    
                 
    // Remember to flush the output to ensure the last action is written to file.
    fout.flush();
}


int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
