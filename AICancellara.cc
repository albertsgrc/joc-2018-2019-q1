#include <cassert>
#include <list>
#include "Player.hh"

#define PLAYER_NAME Cancellara

#define ensure(condition, message) { if (not (condition)) { cerr << "ERROR: " << (message) << endl; assert(condition); } }

typedef const Pos& P;
typedef Dir D;

inline int operator~(P pos) {
    return pos.i*60 + pos.j;
}

struct PLAYER_NAME : public Player {

    static Player *factory() {
        return new PLAYER_NAME;
    }

    enum Owner {
        Own, Enemy, Anyone
    };

    enum Safety {
        Safe, Unsafe
    };

    enum UseState {
        NotTaken, DoesntMatterTaken
    };

    enum Adjacency {
        Adjacent, ExactlyThere
    };

    struct PathInfo;



//    ██████╗ ██╗██████╗ ███████╗ ██████╗████████╗██╗ ██████╗ ███╗   ██╗███████╗
//    ██╔══██╗██║██╔══██╗██╔════╝██╔════╝╚══██╔══╝██║██╔═══██╗████╗  ██║██╔════╝
//    ██║  ██║██║██████╔╝█████╗  ██║        ██║   ██║██║   ██║██╔██╗ ██║███████╗
//    ██║  ██║██║██╔══██╗██╔══╝  ██║        ██║   ██║██║   ██║██║╚██╗██║╚════██║
//    ██████╔╝██║██║  ██║███████╗╚██████╗   ██║   ██║╚██████╔╝██║ ╚████║███████║
//    ╚═════╝ ╚═╝╚═╝  ╚═╝╚══════╝ ╚═════╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝

    // Do not use this array
    const D c_dirs[8] = {
        Bottom, BR, Right, RT, Top, TL, Left, LB
    };

    // We iterate this array to find possible movements from a given position
    D dirs[8];



//    ██╗  ██╗███████╗██╗     ██████╗ ███████╗██████╗ ███████╗
//    ██║  ██║██╔════╝██║     ██╔══██╗██╔════╝██╔══██╗██╔════╝
//    ███████║█████╗  ██║     ██████╔╝█████╗  ██████╔╝███████╗
//    ██╔══██║██╔══╝  ██║     ██╔═══╝ ██╔══╝  ██╔══██╗╚════██║
//    ██║  ██║███████╗███████╗██║     ███████╗██║  ██║███████║
//    ╚═╝  ╚═╝╚══════╝╚══════╝╚═╝     ╚══════╝╚═╝  ╚═╝╚══════╝

    template <class C, class K>
    auto in_impl(C const& c, K const& key, int )
    -> decltype(c.find(key), true) {
        return c.find(key) != c.end();
    }

    // the general case
    template <class C, class K>
    bool in_impl(C const& c, K const& key, ...) {
        using std::begin; using std::end;
        return std::find(begin(c), end(c), key) != end(c);
    }

    template <class Container, class Key>
    bool in(Container const& c, Key const& key) {
        return in_impl(c, key, 0);
    }

    template<typename T>
    bool one_of(const T& value) {
        return false;
    }

    template<typename V, typename... T>
    bool one_of(const V& value, const V& value2, T... values) {
        return value == value2 or one_of(value, values...);
    }

    inline int random(float probability) {
        return Random_generator::random(1, 1000) <= probability*1000;
    }




//    ██████╗ █████╗ ███╗   ██╗    ███╗   ███╗ ██████╗ ██╗   ██╗███████╗██████╗
//    ██╔════╝██╔══██╗████╗  ██║    ████╗ ████║██╔═══██╗██║   ██║██╔════╝╚════██╗
//    ██║     ███████║██╔██╗ ██║    ██╔████╔██║██║   ██║██║   ██║█████╗    ▄███╔╝
//    ██║     ██╔══██║██║╚██╗██║    ██║╚██╔╝██║██║   ██║╚██╗ ██╔╝██╔══╝    ▀▀══╝
//    ╚██████╗██║  ██║██║ ╚████║    ██║ ╚═╝ ██║╚██████╔╝ ╚████╔╝ ███████╗  ██╗
//    ╚═════╝╚═╝  ╚═╝╚═╝  ╚═══╝    ╚═╝     ╚═╝ ╚═════╝   ╚═══╝  ╚══════╝  ╚═╝

    inline bool can_warriors_move() {
        return round() % 4 == me();
    }

    // Assumes round()%4 == me()
    // Assumes pos is adjacent to id
    // TODO: Moving to a position where there is possibly going to be a friendly unit should not be possible
    inline bool can_move_to(const Unit& unit, P pos, Safety safety, UseState useState = DoesntMatterTaken) {
        if (not pos_ok(pos)) return false;

        //ensure(is_adjacent_to(unit, pos), "can_move_to with non-adjacent position");
        ensure((unit.type == Car and can_move(unit.id)) or (unit.type == Warrior and can_warriors_move()), "can_move_to for warrior on invalid round");

        if (useState == NotTaken and is_taken(pos)) return false;

        if (is_on(pos, Own)) return false;

        if (unit.type == Car and is_on(pos, Enemy, Car)) return false;

        if (safety == Safe and is_on(pos, Enemy)) return false;


        switch (unit.type) {
            case Warrior: return is_any(pos, { Desert, City, Road });
            case Car: return can_move(unit.id) and is_any(pos, { Desert, Road });
            default: _unreachable();
        }
    }



//    ██████╗  ██████╗ ███████╗    ██╗███╗   ██╗███████╗ ██████╗
//    ██╔══██╗██╔═══██╗██╔════╝    ██║████╗  ██║██╔════╝██╔═══██╗
//    ██████╔╝██║   ██║███████╗    ██║██╔██╗ ██║█████╗  ██║   ██║
//    ██╔═══╝ ██║   ██║╚════██║    ██║██║╚██╗██║██╔══╝  ██║   ██║
//    ██║     ╚██████╔╝███████║    ██║██║ ╚████║██║     ╚██████╔╝
//    ╚═╝      ╚═════╝ ╚══════╝    ╚═╝╚═╝  ╚═══╝╚═╝      ╚═════╝

    // Adjacency

    inline bool is_adjacent_to(P pos, const function<bool(P)>& evaluator) {
        for (const D& d : dirs) {
            Pos dest = pos + d;
            if (pos_ok(dest) and evaluator(dest)) return true;
        }
        return false;
    }

    inline bool is_adjacent_to(P pos, P other_pos) {
        return is_adjacent_to(pos, [&other_pos](P p) { return p == other_pos; });
    }

    inline bool is_adjacent_to(P pos, CellType type) {
        return is_adjacent_to(pos, [type, this](P p) { return is(p, type); });
    }


    inline bool is_adjacent_to(P pos, Owner owner) {
        return is_adjacent_to(pos, [owner, this](P p) { return is(p, owner); });
    }

    inline bool is_adjacent_to(P pos, int other_unit_id) {
        return is_adjacent_to(pos, [other_unit_id, this](P p) { return cell(p).id == other_unit_id; });
    }

    inline bool is_adjacent_to(P pos, UnitType type) {
        return is_adjacent_to(pos, [type, this](P p) {
            Cell c = cell(p);
            return c.id != -1 and unit(c.id).type == type;
        });
    }

    // Has a unit of given type?

    inline bool has_unit(P pos, UnitType type = UnitTypeSize) {
        if (type == UnitTypeSize) {
            return cell(pos).id != -1;
        } else {
            int unit_id = cell(pos).id;
            return cell(pos).id != -1 and unit(unit_id).type == type;
        }
    }

    // Is it of a given cell type?

    inline bool is(P pos, CellType cellType) {
        return cell(pos).type == cellType;
    }

    // Is it a city of a given owner?

    inline bool is(P pos, Owner owner) {
        if(!is(pos, City)) return owner == Anyone;
        else {
            int owner_id = cell(pos).owner;
            if (owner == Own) return owner_id == me();
            else if (owner == Enemy) return owner_id != me();
            else return false;
        }
    }

    // Is it any of the following cell types?

    inline bool is_any(P pos, initializer_list<CellType> list) {
        for (auto type : list) {
            if (is(pos, type)) return true;
        }
        return false;
    }


    inline bool is_on(P pos, Owner owner, UnitType type = UnitTypeSize) {
        bool has = has_unit(pos, type);

        if (not has) return false;

        int player = unit(cell(pos).id).player;

        if (owner == Own) return player == me();
        else return player != me();
    }


    inline Unit unit_pos(P pos) {
        Cell c = cell(pos);

        ensure(c.id != -1, "Get unit from pos without any unit");

        return Info::unit(c.id);
    }

    inline bool is_taken(P pos) {
        return in(used_positions, ~pos);
    }

//    ██████╗ ██████╗ ███╗   ███╗███╗   ███╗ █████╗ ███╗   ██╗██████╗      ██████╗ ██████╗ ███╗   ██╗████████╗██████╗  ██████╗ ██╗
//    ██╔════╝██╔═══██╗████╗ ████║████╗ ████║██╔══██╗████╗  ██║██╔══██╗    ██╔════╝██╔═══██╗████╗  ██║╚══██╔══╝██╔══██╗██╔═══██╗██║
//    ██║     ██║   ██║██╔████╔██║██╔████╔██║███████║██╔██╗ ██║██║  ██║    ██║     ██║   ██║██╔██╗ ██║   ██║   ██████╔╝██║   ██║██║
//    ██║     ██║   ██║██║╚██╔╝██║██║╚██╔╝██║██╔══██║██║╚██╗██║██║  ██║    ██║     ██║   ██║██║╚██╗██║   ██║   ██╔══██╗██║   ██║██║
//    ╚██████╗╚██████╔╝██║ ╚═╝ ██║██║ ╚═╝ ██║██║  ██║██║ ╚████║██████╔╝    ╚██████╗╚██████╔╝██║ ╚████║   ██║   ██║  ██║╚██████╔╝███████╗
//    ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚═════╝      ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝ ╚══════╝

    struct Action {
        int unit_id;
        D dir;
        float importance;

        Action(int unit_id, D dir, float importance) : unit_id(unit_id), dir(dir), importance(importance) {}

        bool operator<(const Action& other) const {
            return importance < other.importance;
        }
    };

    priority_queue<Action> actions_queue;
    set<int> units_to_command;
    set<int> used_positions;

    // Call this method instead of command
    void action(int unit_id, D dir, float importance) {
        actions_queue.push(Action(unit_id, dir, importance));
    }

    void action(const Unit& unit, D dir, float importance) {
        action(unit.id, dir, importance);
    }

    void action(const Unit& unit, const PathInfo& info, float importance) {
        action(unit.id, info.dir, importance);
    }

    void process_actions() {
        while (!actions_queue.empty()) {
            Action action = actions_queue.top(); actions_queue.pop();

            ensure(dir_ok(action.dir), "try to process action with invalid dir");

            Pos destination = unit(action.unit_id).pos + action.dir;

            if (used_positions.insert(~destination).second) {
                // Was not a used position, perform command
                ensure(cell(destination).id == -1 or unit(cell(destination).id).player != me(), "attacking own unit!");

                //cerr << "Unit " << action.unit_id << " to " << destination << endl;
                command(action.unit_id, action.dir);
                units_to_command.erase(action.unit_id);
            }
        }
    }

    void set_inactive_round(const Unit &unit) {
        units_to_command.erase(unit.id);
    }

    bool already_commanded(const Unit& unit) {
        return not in(units_to_command, unit.id);
    }



//    ███████╗████████╗ █████╗ ████████╗███████╗
//    ██╔════╝╚══██╔══╝██╔══██╗╚══██╔══╝██╔════╝
//    ███████╗   ██║   ███████║   ██║   █████╗
//    ╚════██║   ██║   ██╔══██║   ██║   ██╔══╝
//    ███████║   ██║   ██║  ██║   ██║   ███████╗
//    ╚══════╝   ╚═╝   ╚═╝  ╚═╝   ╚═╝   ╚══════╝


    typedef vector<int> VI;
    typedef vector<bool> VB;
    typedef vector<VB> VVB;

    VI my_car_ids;
    VI my_warrior_ids;



//    ██████╗  █████╗ ████████╗██╗  ██╗███████╗██╗███╗   ██╗██████╗ ██╗███╗   ██╗ ██████╗
//    ██╔══██╗██╔══██╗╚══██╔══╝██║  ██║██╔════╝██║████╗  ██║██╔══██╗██║████╗  ██║██╔════╝
//    ██████╔╝███████║   ██║   ███████║█████╗  ██║██╔██╗ ██║██║  ██║██║██╔██╗ ██║██║  ███╗
//    ██╔═══╝ ██╔══██║   ██║   ██╔══██║██╔══╝  ██║██║╚██╗██║██║  ██║██║██║╚██╗██║██║   ██║
//    ██║     ██║  ██║   ██║   ██║  ██║██║     ██║██║ ╚████║██████╔╝██║██║ ╚████║╚██████╔╝
//    ╚═╝     ╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝╚═╝     ╚═╝╚═╝  ╚═══╝╚═════╝ ╚═╝╚═╝  ╚═══╝ ╚═════╝

    // This data structure holds the result of a pathfinding call

    struct PathInfo {
        D dir; // Direction where you need to go
        int dist; // Distance to get to the destination
        int rounds; // Rounds to get to the destination
        Pos dest; // not useful

        PathInfo(D dir, int dist, int rounds, Pos dest) : dir(dir), dist(dir), dest(dest) {}
        PathInfo() : dist(-1) {}

        bool found() { // Returns whether a path was found
            return dist != -1;
        }
    };

    // TODO: Take in account that distances for cars are different when they are on a road
    PathInfo bfs(
            P initial,
            const function<bool(P)>& can_go,
            const function<bool(P)>& is_dest,
            int max_dist = INT_MAX,
            bool first_allowed = false)
    {
        if (is_dest(initial) and first_allowed) return { None, 0, 0, initial };

        queue<PathInfo> queue;
        VVB seen(rows(), VB(cols(), false)); seen[initial.i][initial.j] = true;

        for (D dir : dirs) {
            Pos dest = initial + dir;

            if (can_go(dest) and not is_taken(dest)) {
                seen[dest.i][dest.j] = true;
                queue.push(PathInfo(dir, 1, 1, dest));
            }
        }

        while (not queue.empty() and not is_dest(queue.front().dest)) {
            PathInfo pathInfoOrigin = queue.front();
            queue.pop();

            for (D dir : dirs) {
                Pos dest = pathInfoOrigin.dest + dir;

                if (can_go(dest) and not seen[dest.i][dest.j]) {
                    seen[dest.i][dest.j] = true;

                    if (pathInfoOrigin.dist + 1 <= max_dist) {
                        queue.push(PathInfo(pathInfoOrigin.dir, pathInfoOrigin.dist + 1, 1, dest));
                    }
                }
            }
        }

        if (not queue.empty()) return queue.front();
        else return {};

    }

    PathInfo bfs(
            const Unit& unit,
            const function<bool(P)>& can_go,
            const function<bool(P)>& is_dest,
            int max_dist = INT_MAX,
            bool first_allowed = false)
    {
        return bfs(unit.pos, can_go, is_dest, max_dist, first_allowed);
    }

    PathInfo bfs(
            const Unit& unit,
            const function<bool(P)>& is_dest,
            Safety safety,
            int max_dist = INT_MAX,
            bool first_allowed = false)
    {
        return bfs(unit, [&unit, this, safety](P pos) { return can_move_to(unit, pos, safety); }, is_dest, max_dist, first_allowed);
    }


    PathInfo bfs(const Unit& unit, P dest, Safety safety, int maxdist = INT_MAX) {
        return bfs(
                unit.pos,
                [&unit, this, safety](P pos) { return can_move_to(unit, pos, safety); },
                [&dest](P pos) { return dest == pos; },
                maxdist
                );
    }

    PathInfo bfs(
            const Unit& unit,
            Safety safety,
            Owner owner,
            UnitType type = UnitTypeSize,
            int max_dist = INT_MAX,
            bool first_allowed = false)
    {
        return bfs(
                unit,
                [&unit, this, safety](P pos) { return can_move_to(unit, pos, safety); },
                [&unit, owner, type, this](P pos) { return is_on(pos, owner, type); },
                max_dist,
                first_allowed
        );
    }

    PathInfo bfs(const Unit& unit, Safety safety, Adjacency adjacency, CellType type, int maxdist = INT_MAX) {
        function<bool(P)> is_dest;

        if (adjacency == Adjacent) is_dest = [type, this](P pos) { return is_adjacent_to(pos, type); };
        else is_dest = [type, this](P pos) { return is(pos, type); };


        return bfs(
                unit.pos,
                [&unit, this, safety](P pos) { return can_move_to(unit, pos, safety); },
                is_dest,
                maxdist
        );
    }

    PathInfo bfs(const Unit& unit, Safety safety, Adjacency adjacency, Owner owner, int maxdist = INT_MAX) {
        function<bool(P)> is_dest;

        if (adjacency == Adjacent) is_dest = [owner, this](P pos) { return is_adjacent_to(pos, owner); };
        else is_dest = [owner, this](P pos) { return is(pos, owner); };


        return bfs(
                unit.pos,
                [&unit, this, safety](P pos) { return can_move_to(unit, pos, safety); },
                is_dest,
                maxdist
        );
    }



//    ██████╗ ███████╗ █████╗ ██╗         ███████╗██╗  ██╗██╗████████╗
//    ██╔══██╗██╔════╝██╔══██╗██║         ██╔════╝██║  ██║██║╚══██╔══╝
//    ██████╔╝█████╗  ███████║██║         ███████╗███████║██║   ██║
//    ██╔══██╗██╔══╝  ██╔══██║██║         ╚════██║██╔══██║██║   ██║
//    ██║  ██║███████╗██║  ██║███████╗    ███████║██║  ██║██║   ██║
//    ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚══════╝    ╚══════╝╚═╝  ╚═╝╚═╝   ╚═╝

    void compute_action_warrior(const Unit& warrior) {
        cerr << "Warrior " << warrior.id << ": " << warrior.food << ' ' << warrior.water << endl;
        PathInfo water = bfs(warrior, Safe, Adjacent, Water);

        PathInfo city = bfs(warrior, Safe, ExactlyThere, Anyone);
        PathInfo enemy_city = bfs(warrior, Unsafe, ExactlyThere, Enemy);
        PathInfo enemy_warrior = bfs(warrior, Unsafe, Enemy, Warrior);

        if (water.dist >= warrior.water - 25 and city.dist < warrior.food - 20) {
            cerr << "Warrior going for wateeeer" << endl;
            action(warrior, water, 5);
        }
        else if (city.found() and city.dist >= warrior.food - 15) {
            cerr << "Warrior going for own city" << endl;
            action(warrior, city, 5);
        }
        else if (enemy_warrior.found() and warrior.water > unit_pos(enemy_warrior.dest).water and random(0.5)) {
            cerr << "Warrior going for enemy" << endl;
            action(warrior, enemy_warrior, 2);
        }
        else {
            cerr << "Warrior going for enemy city" << endl;
            action(warrior, enemy_city, (float(round())/nb_rounds())*10);
        }
    }

    void compute_action_car(const Unit& car) {
        if (not can_move(car.id)) return set_inactive_round(car);

        PathInfo station = bfs(car, Safe, Adjacent, Station);
        PathInfo enemy_warrior = bfs(car, Unsafe, Enemy, Warrior);
        PathInfo enemy_city = bfs(car, Unsafe, Adjacent, Enemy);

        if (station.dist >= car.food) action(car, station, 3);
        else if (enemy_warrior.found()) action(car, enemy_warrior, 2);
        else action(car, enemy_city, 1);
    }


    void compute_actions() {
        list<int> units_list(units_to_command.begin(), units_to_command.end());

        for (int unit_id : units_list) {
            Unit m_unit = unit(unit_id);
            if (m_unit.type == Warrior) compute_action_warrior(m_unit);
            else compute_action_car(m_unit);
        }
    }



//    ██████╗  ██████╗ ██╗   ██╗███╗   ██╗██████╗     ██╗███╗   ██╗██╗████████╗
//    ██╔══██╗██╔═══██╗██║   ██║████╗  ██║██╔══██╗    ██║████╗  ██║██║╚══██╔══╝
//    ██████╔╝██║   ██║██║   ██║██╔██╗ ██║██║  ██║    ██║██╔██╗ ██║██║   ██║
//    ██╔══██╗██║   ██║██║   ██║██║╚██╗██║██║  ██║    ██║██║╚██╗██║██║   ██║
//    ██║  ██║╚██████╔╝╚██████╔╝██║ ╚████║██████╔╝    ██║██║ ╚████║██║   ██║
//    ╚═╝  ╚═╝ ╚═════╝  ╚═════╝ ╚═╝  ╚═══╝╚═════╝     ╚═╝╚═╝  ╚═══╝╚═╝   ╚═╝

    // Shuffles the directions array
    // I don't remember why but this produces better warrior/car movements overall
    void shuffle_dirs() {
        int i = 0;
        VI permutation = random_permutation(8);

        for (D& dir : dirs) dir = c_dirs[permutation[i++]];
    }


    void get_my_units() {
        my_car_ids = cars(me());
        my_warrior_ids = warriors(me());
    }

//    ███████╗██╗██████╗ ███████╗████████╗    ██╗███╗   ██╗██╗████████╗
//    ██╔════╝██║██╔══██╗██╔════╝╚══██╔══╝    ██║████╗  ██║██║╚══██╔══╝
//    █████╗  ██║██████╔╝███████╗   ██║       ██║██╔██╗ ██║██║   ██║
//    ██╔══╝  ██║██╔══██╗╚════██║   ██║       ██║██║╚██╗██║██║   ██║
//    ██║     ██║██║  ██║███████║   ██║       ██║██║ ╚████║██║   ██║
//    ╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝   ╚═╝       ╚═╝╚═╝  ╚═══╝╚═╝   ╚═╝


    // Called only on the first round to perform some initialization




//    ██████╗ ██╗      █████╗ ██╗   ██╗
//    ██╔══██╗██║     ██╔══██╗╚██╗ ██╔╝
//    ██████╔╝██║     ███████║ ╚████╔╝
//    ██╔═══╝ ██║     ██╔══██║  ╚██╔╝
//    ██║     ███████╗██║  ██║   ██║
//    ╚═╝     ╚══════╝╚═╝  ╚═╝   ╚═╝

    void play() {
        //if (round() == 0)

        get_my_units();

        shuffle_dirs();

        used_positions.clear();

        if (can_warriors_move()) units_to_command.insert(my_warrior_ids.begin(), my_warrior_ids.end());
        units_to_command.insert(my_car_ids.begin(), my_car_ids.end());

        do {
            compute_actions();
            process_actions();
        } while(not units_to_command.empty());
    }
};

RegisterPlayer(PLAYER_NAME);
