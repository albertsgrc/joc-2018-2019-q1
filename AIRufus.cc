#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME InnerBloom


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */
    typedef vector< vector<Cell> > matrix;
    typedef pair<int,Pos> travel;
    typedef vector< vector<int> > valuedMap;

    void read_map(matrix &Map, valuedMap &VC, valuedMap &VW ) {
        for (int i=0; i<59; ++i) {
            for (int j=0; j<59; ++j) {
                Pos p(i,j);
                Map[i][j] = cell(p);
                //avalueCarPos(Map[i][j],VC);
                //avalueWarriorPos(Map[i][j],VW);
            }
        }
    }

    //Gives a value to a position as a possible car destiny
    void avalueCarPos(const Cell &c, valuedMap &v) {

    }

    //Gives a value to a position as a possible warrior destiny
    void avalueWarriorPos (const Cell &c, valuedMap &v) {

    }

    //Returns false if there's a troop of mine
    bool itsNotMe(const Pos &p) {
        Cell c = cell(p);
        return cell(p).id != -1 and unit(c.id).player != me();
    }

    Dir computeDir(const Unit &unit, const Pos &dest) {
        Dir res;
        Pos origin = unit.pos;
        int oi = origin.i;
        int oj = origin.j;
        int di = dest.i;
        int dj = dest.j;
        if ( di == oi-1 and dj == oj-1) return LB;
        else if (di == oi and dj == oj-1) return Bottom;
        else if (di == oi+1 and dj == oj-1) return BR;
        else if (di == oi+1 and dj==oj) return Right;
        else if (di == oi+1 and dj==oj+1) return RT;
        else if (di == oi and dj==oj+1) return Top;
        else if (di == oi-1 and dj==oj+1) return TL;
        else return Left;
    }

    /*
        Pre: It gets the position of the next step a warrior would do
        Post: Return "false" if: 
                > Invalid pos
                > There's water
                > There's me
    */
    bool validate_pos_warrior(const Pos &nextStep) {
        Cell c = cell(nextStep);
        if (nextStep.i >= 60 or nextStep.j >= 60) return false;
        else if (c.type == Water or c.type == Wall or !itsNotMe(nextStep)) {
            return false;
        }
        return true;
    }

    queue<Pos> warrior_decider(const matrix &Map, const int &id) {
        int n = Map.size();
        priority_queue< travel, vector<travel>, greater<travel> > Q;
        vector< vector<int> > distances(n, vector<int>(n,999999999));
        vector< vector<bool> > path(n, vector<bool>(n, false));

        queue<Pos> steps;

        Pos origin = unit(id).pos;
        Unit war = unit(cell(origin).id);
        distances[origin.i][origin.j] = 0;

        travel t(0, origin);
        Q.push(t);
        while (not Q.empty()) {
            Pos actPos = Q.top().second;
            Q.pop();
            if (not path[actPos.i][actPos.j]) {
                path[origin.i][origin.j] = true;            
                Cell actCell = cell(actPos);

                //Decider
                if (distances[actPos.i][actPos.j]>0) {
                    if (actCell.id == -1) {
                    //No hi ha cap tropa
                    if (actCell.type == City) {
                        if (actCell.owner != me()) steps.push(actPos);
                        else {
                            steps.push(actPos);
                        }
                    }
                    } else {
                        //Hi ha una tropa i no es meva
                        Unit enemy = unit(actCell.id);
                        if (enemy.food%6 < war.food%6 and enemy.water%6 < war.water%6) steps.push(actPos);
                    }
                }
                

                //Aquí validem les caselles del mapa al voltant nostre
                //per decidir tractar-les després
                for (int i=-1; i<1; ++i){
                    for (int j=-1; j<1; ++j) {
                        if (!(j==0 and i==0)) {
                            Pos nextStep(actPos.i+i,actPos.j+j);
                            if (validate_pos_warrior(nextStep)) {
                                distances[nextStep.i][nextStep.j] = distances[actPos.i][actPos.i]+1;
                                Q.push(travel(distances[nextStep.i][nextStep.j], nextStep));
                            }
                        }
                    }
                }
            }
        }
        return steps;
    }

    void command_cars(const matrix &Map) {
        vector<int> Cars = cars(me());
    }

    void command_warriors(const matrix &Map) {
        vector<int> army = warriors(me());
        int n = army.size();

        Pos nextPos;
        for (int i=0; i<n; ++i) {
            nextPos = warrior_decider(Map,army[i]).front();
            Dir d = computeDir(unit(army[i]),nextPos);
            command(army[i],d);
        }
    }

  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
        vector< vector<Cell> > Map(60, vector<Cell>(60));
        valuedMap valuedCarMap;
        valuedMap valuedWarriorMap;
        read_map(Map,valuedCarMap,valuedWarriorMap);
        command_warriors(Map);
        command_cars(Map);
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
