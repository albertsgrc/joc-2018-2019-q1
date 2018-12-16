#include <cassert>
#include <list>
#include <climits>
#include <functional>
#include <cstring>
#include <unordered_map>
#include "Player.hh"

#define PLAYER_NAME Jutge
#define IS_DEBUG 0

// region defines
// ██████╗ ███████╗███████╗██╗███╗   ██╗███████╗███████╗
// ██╔══██╗██╔════╝██╔════╝██║████╗  ██║██╔════╝██╔════╝
// ██║  ██║█████╗  █████╗  ██║██╔██╗ ██║█████╗  ███████╗
// ██║  ██║██╔══╝  ██╔══╝  ██║██║╚██╗██║██╔══╝  ╚════██║
// ██████╔╝███████╗██║     ██║██║ ╚████║███████╗███████║
// ╚═════╝ ╚══════╝╚═╝     ╚═╝╚═╝  ╚═══╝╚══════╝╚══════╝


#if IS_DEBUG
#define ensure(condition, message) { if (not (condition)) { cerr << "ERROR: " << (message) << endl; assert(condition); } }
#else
#define ensure(condition, message) {}
#endif

#define cinfo (*this)

// endregion


// region typedefs
//████████╗██╗   ██╗██████╗ ███████╗██████╗ ███████╗███████╗███████╗
//╚══██╔══╝╚██╗ ██╔╝██╔══██╗██╔════╝██╔══██╗██╔════╝██╔════╝██╔════╝
//   ██║    ╚████╔╝ ██████╔╝█████╗  ██║  ██║█████╗  █████╗  ███████╗
//   ██║     ╚██╔╝  ██╔═══╝ ██╔══╝  ██║  ██║██╔══╝  ██╔══╝  ╚════██║
//   ██║      ██║   ██║     ███████╗██████╔╝███████╗██║     ███████║
//   ╚═╝      ╚═╝   ╚═╝     ╚══════╝╚═════╝ ╚══════╝╚═╝     ╚══════╝


typedef unsigned short number;
typedef unsigned char snumber;

struct Direction {
    char i, j;

    Direction() : i(0), j(0) {}
    Direction(int i, int j) : i(i), j(j) {}

    Direction(const Direction& o) : i(o.i), j(o.j) {}

    explicit operator Dir() const {
        switch(i) {
            case -1:
                switch(j) {
                    case -1: return TL;
                    case 0: return Top;
                    case 1: return RT;
                }
            case 0:
                switch(j) {
                    case -1: return Left;
                    case 0: return None;
                    case 1: return Right;
                }
            case 1:
                switch(j) {
                    case -1: return LB;
                    case 0: return Bottom;
                    case 1: return BR;
                }
        }

        _unreachable();
    }

    inline Pos operator+(const Pos& pos) {
        return {pos.i + i, pos.j + j};
    }

    friend ostream& operator<<(ostream& o, const Direction& d) {
        o << '(' << int(d.i) << ',' << int(d.j) << ')';
        return o;
    }
};

inline Pos operator+(const Pos& pos, const Direction& dir) {
    return {pos.i + dir.i, pos.j + dir.j};
}

typedef const Pos& P;
typedef const Direction& D;

// endregion


// region operators
//  ██████╗ ██████╗ ███████╗██████╗  █████╗ ████████╗ ██████╗ ██████╗ ███████╗
// ██╔═══██╗██╔══██╗██╔════╝██╔══██╗██╔══██╗╚══██╔══╝██╔═══██╗██╔══██╗██╔════╝
// ██║   ██║██████╔╝█████╗  ██████╔╝███████║   ██║   ██║   ██║██████╔╝███████╗
// ██║   ██║██╔═══╝ ██╔══╝  ██╔══██╗██╔══██║   ██║   ██║   ██║██╔══██╗╚════██║
// ╚██████╔╝██║     ███████╗██║  ██║██║  ██║   ██║   ╚██████╔╝██║  ██║███████║
//  ╚═════╝ ╚═╝     ╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝╚══════╝


inline number operator~(P pos) {
    return pos.i*60 + pos.j;
}


// endregion

struct PLAYER_NAME : public Player {

    static Player *factory() {
        return new PLAYER_NAME;
    }

    // region enums
    // ███████╗███╗   ██╗██╗   ██╗███╗   ███╗███████╗
    // ██╔════╝████╗  ██║██║   ██║████╗ ████║██╔════╝
    // █████╗  ██╔██╗ ██║██║   ██║██╔████╔██║███████╗
    // ██╔══╝  ██║╚██╗██║██║   ██║██║╚██╔╝██║╚════██║
    // ███████╗██║ ╚████║╚██████╔╝██║ ╚═╝ ██║███████║
    // ╚══════╝╚═╝  ╚═══╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝

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

    enum Resource {
        Drink, Fuel
    };

    struct PathInfo;

    // endregion


    // region directions
    // ██████╗ ██╗██████╗ ███████╗ ██████╗████████╗██╗ ██████╗ ███╗   ██╗███████╗
    // ██╔══██╗██║██╔══██╗██╔════╝██╔════╝╚══██╔══╝██║██╔═══██╗████╗  ██║██╔════╝
    // ██║  ██║██║██████╔╝█████╗  ██║        ██║   ██║██║   ██║██╔██╗ ██║███████╗
    // ██║  ██║██║██╔══██╗██╔══╝  ██║        ██║   ██║██║   ██║██║╚██╗██║╚════██║
    // ██████╔╝██║██║  ██║███████╗╚██████╗   ██║   ██║╚██████╔╝██║ ╚████║███████║
    // ╚═════╝ ╚═╝╚═╝  ╚═╝╚══════╝ ╚═════╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝


    // Do not use this array
    const Direction c_dirs[8] = {
            {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1},{0,-1},{1,-1}
    };


    const Direction c_dirs_all[9] = {
            {0,0}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1},{0,-1},{1,-1}
    };

    // We iterate this array to find possible movements from a given position
    Direction dirs[8];

    //endregion


    // region helpers
    // ██╗  ██╗███████╗██╗     ██████╗ ███████╗██████╗ ███████╗
    // ██║  ██║██╔════╝██║     ██╔══██╗██╔════╝██╔══██╗██╔════╝
    // ███████║█████╗  ██║     ██████╔╝█████╗  ██████╔╝███████╗
    // ██╔══██║██╔══╝  ██║     ██╔═══╝ ██╔══╝  ██╔══██╗╚════██║
    // ██║  ██║███████╗███████╗██║     ███████╗██║  ██║███████║
    // ╚═╝  ╚═╝╚══════╝╚══════╝╚═╝     ╚══════╝╚═╝  ╚═╝╚══════╝

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

    inline bool random(float probability) {
        return Random_generator::random(1, 1000) <= probability*1000;
    }

    //endregion


    //region can-move
    //  ██████╗ █████╗ ███╗   ██╗    ███╗   ███╗ ██████╗ ██╗   ██╗███████╗██████╗
    // ██╔════╝██╔══██╗████╗  ██║    ████╗ ████║██╔═══██╗██║   ██║██╔════╝╚════██╗
    // ██║     ███████║██╔██╗ ██║    ██╔████╔██║██║   ██║██║   ██║█████╗    ▄███╔╝
    // ██║     ██╔══██║██║╚██╗██║    ██║╚██╔╝██║██║   ██║╚██╗ ██╔╝██╔══╝    ▀▀══╝
    // ╚██████╗██║  ██║██║ ╚████║    ██║ ╚═╝ ██║╚██████╔╝ ╚████╔╝ ███████╗  ██╗
    //  ╚═════╝╚═╝  ╚═╝╚═╝  ╚═══╝    ╚═╝     ╚═╝ ╚═════╝   ╚═══╝  ╚══════╝  ╚═╝

    inline bool pos_ok(const Pos& pos) {
        return (pos.i >= 0) & (pos.j >= 0) & (pos.i < 60) & (pos.j < 60);
    }

    inline bool can_warriors_move() {
        return round() % 4 == me();
    }

    inline bool can_move_to_simple_warrior(P pos) {
        return pos_ok(pos) && (cell_type[pos.i][pos.j] <= City);
    }

    inline bool can_move_to_simple_car(P pos) {
        return pos_ok(pos) && (cell_type[pos.i][pos.j] <= Road);
    }

    // Assumes round()%4 == me()
    // Assumes pos is adjacent to id
    inline bool can_move_to(const Unit& unit, P pos, Safety safety, UseState useState = DoesntMatterTaken) {
        if (not pos_ok(pos)) return false;

        //ensure(is_adjacent_to(unit, pos), "can_move_to with non-adjacent position");
        ensure((unit.type == Car and can_move(unit.id)) or (unit.type == Warrior and can_warriors_move()), "can_move_to for warrior on invalid round");

        if (useState == NotTaken and not can_demand(unit, pos)) return false;

        if (is_on(pos, Own)) return false;

        if (unit.type == Car and (is_on(pos, Enemy, Car) or cinfo[pos].enemy_cars > 0)) return false;

        if (unit.type == Warrior and safety == Safe and cinfo[pos].enemy_warriors > 0) return false;


        switch (unit.type) {
            case Warrior: return cell_type[pos.i][pos.j] <= City;
            case Car: return cell_type[pos.i][pos.j] <= Road;
            default: _unreachable();
        }
    }

    //endregion


    //region position-info
    // ██████╗  ██████╗ ███████╗    ██╗███╗   ██╗███████╗ ██████╗
    // ██╔══██╗██╔═══██╗██╔════╝    ██║████╗  ██║██╔════╝██╔═══██╗
    // ██████╔╝██║   ██║███████╗    ██║██╔██╗ ██║█████╗  ██║   ██║
    // ██╔═══╝ ██║   ██║╚════██║    ██║██║╚██╗██║██╔══╝  ██║   ██║
    // ██║     ╚██████╔╝███████║    ██║██║ ╚████║██║     ╚██████╔╝
    // ╚═╝      ╚═════╝ ╚══════╝    ╚═╝╚═╝  ╚═══╝╚═╝      ╚═════╝

    // Adjacency

    inline bool is_adjacent_to(P pos, const function<bool(P)>& evaluator) {
        for (D d : dirs) {
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

    inline bool is_adjacent_to(P pos, snumber other_unit_id) {
        return is_adjacent_to(pos, [other_unit_id, this](P p) { return cell(p).id == other_unit_id; });
    }

    inline bool is_adjacent_to(P pos, Owner owner, UnitType type) {
        return is_adjacent_to(pos, [type, owner, this](P p) {
            return is_on(p, owner, type);
        });
    }

    // Has a unit of given type?

    inline bool has_unit(P pos, UnitType type = UnitTypeSize) {
        if (type == UnitTypeSize) {
            return cell(pos).id != -1;
        } else {
            snumber unit_id = cell(pos).id;
            return cell(pos).id != -1 and unit(unit_id).type == type;
        }
    }

    // Is it of a given cell type?

    inline bool is(P pos, CellType cellType) {
        return cell_type[pos.i][pos.j] == cellType;
    }

    inline bool has(P pos, Resource resource) {
        if (resource == Drink) return cinfo[pos].gives_water;
        else return cinfo[pos].gives_fuel;
    }

    // Is it a city of a given owner?

    inline bool is(P pos, Owner owner) {
        if(!is(pos, City)) return owner == Anyone;
        else {
            snumber owner_id = cell(pos).owner;
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

        snumber player = unit(cell(pos).id).player;

        if (owner == Own) return player == me();
        else return player != me();
    }


    inline Unit unit_pos(P pos) {
        Cell c = cell(pos);

        ensure(c.id != -1, "Get unit from pos without any unit");

        return Info::unit(c.id);
    }

    inline bool is_thunderdome(P pos1, P pos2) {
        return pos1 != pos2 and is(pos1, City) and is(pos2, City);
    }

    //endregion


    // region unitinfo
    // ██╗   ██╗███╗   ██╗██╗████████╗    ██╗███╗   ██╗███████╗ ██████╗
    // ██║   ██║████╗  ██║██║╚══██╔══╝    ██║████╗  ██║██╔════╝██╔═══██╗
    // ██║   ██║██╔██╗ ██║██║   ██║       ██║██╔██╗ ██║█████╗  ██║   ██║
    // ██║   ██║██║╚██╗██║██║   ██║       ██║██║╚██╗██║██╔══╝  ██║   ██║
    // ╚██████╔╝██║ ╚████║██║   ██║       ██║██║ ╚████║██║     ╚██████╔╝
    //  ╚═════╝ ╚═╝  ╚═══╝╚═╝   ╚═╝       ╚═╝╚═╝  ╚═══╝╚═╝      ╚═════╝

    inline snumber unit_life(const Unit& u) {
        return min(u.food, u.water);
    }

    inline int rounds_to_move_car(const Unit& u, const Pos& pos, number distance_travelled, bool refueled = false) {
        return (is(pos, Road) and (refueled or (u.food - distance_travelled > 0))) ? 1 : 4;
    }

    inline float win_probability_warrior_thunderdome(const Unit& own, const Unit& enemy) {
        return ((own.water - enemy.water)/float(40) + 1)/2;
    }

    //endregion


    // region actions
    //  █████╗  ██████╗████████╗██╗ ██████╗ ███╗   ██╗███████╗
    // ██╔══██╗██╔════╝╚══██╔══╝██║██╔═══██╗████╗  ██║██╔════╝
    // ███████║██║        ██║   ██║██║   ██║██╔██╗ ██║███████╗
    // ██╔══██║██║        ██║   ██║██║   ██║██║╚██╗██║╚════██║
    // ██║  ██║╚██████╗   ██║   ██║╚██████╔╝██║ ╚████║███████║
    // ╚═╝  ╚═╝ ╚═════╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝

    struct Action {
        snumber unit_id;
        Dir dir;
        float importance;

        Action(snumber unit_id, Dir dir, float importance) : unit_id(unit_id), dir(dir), importance(importance) {}

        bool operator<(const Action& other) const {
            return importance < other.importance;
        }
    };

    priority_queue<Action> actions_queue;
    set<snumber> units_to_command;
    unordered_map<number, snumber> used_positions;

    // Call this method instead of command
    void action(snumber unit_id, Dir dir, float importance) {
        ensure(dir_ok(dir), "try to add action with invalid dir");
        actions_queue.push(Action(unit_id, dir, importance));
    }

    void action(const Unit& unit, Dir dir, float importance) {
        action(unit.id, dir, importance);
    }

    void action(const Unit& unit, D dir, float importance) {
        action(unit.id, Dir(dir), importance);
    }


    void action(const Unit& unit, const PathInfo& info, float importance) {
        action(unit.id, Dir(info.dir), importance);
    }

    void action_command(const Action& action) {
        command(action.unit_id, action.dir);
        units_to_command.erase(action.unit_id);
    }

    void process_actions() {
        while (!actions_queue.empty()) {
            Action action = actions_queue.top(); actions_queue.pop();

            Unit u = unit(action.unit_id);
            Pos actual = u.pos;
            Pos destination = actual + action.dir;

            // TODO: If there is an enemy in the destination and it's not going to die the current position must also be used

            auto insertion = used_positions.insert({ ~destination, 1 });

            if (insertion.second) {
                // Was not a used position, perform command
                //ensure(cell(destination).id == -1 or cell(destination).id == action.unit_id or unit(cell(destination).id).player != me(), "attacking own unit!");

                //cerr << "Unit " << action.unit_id << " to " << destination << endl;
                action_command(action);
            }
            else if (u.type == Warrior and not is_thunderdome(actual, destination) and is_on(destination, Enemy, Warrior)) {
                const Unit& enemy = unit(cell(destination).id);
                snumber life = unit_life(enemy);

                snumber demands = insertion.first->second + 1;

                if (demands*6 < life) {
                    action_command(action);
                }
            }
        }
    }

    void set_inactive_round(const Unit &unit) {
        units_to_command.erase(unit.id);
    }

    bool already_commanded(const Unit& unit) {
        return not in(units_to_command, unit.id);
    }

    inline bool can_demand(const Unit& u, P pos) {
        auto search = used_positions.find(~pos);

        if (search == used_positions.end()) return true;
        else if (not is_thunderdome(u.pos, pos) and is_on(pos, Enemy, Warrior)) {
            const Unit& enemy = unit(cell(pos).id);
            snumber life = min(enemy.water, enemy.food);

            snumber demands = search->second + 1;

            return demands*6 < life;
        }
        else return false;
    }

    // endregion


    // region pathfinding
    // ██████╗  █████╗ ████████╗██╗  ██╗███████╗██╗███╗   ██╗██████╗ ██╗███╗   ██╗ ██████╗
    // ██╔══██╗██╔══██╗╚══██╔══╝██║  ██║██╔════╝██║████╗  ██║██╔══██╗██║████╗  ██║██╔════╝
    // ██████╔╝███████║   ██║   ███████║█████╗  ██║██╔██╗ ██║██║  ██║██║██╔██╗ ██║██║  ███╗
    // ██╔═══╝ ██╔══██║   ██║   ██╔══██║██╔══╝  ██║██║╚██╗██║██║  ██║██║██║╚██╗██║██║   ██║
    // ██║     ██║  ██║   ██║   ██║  ██║██║     ██║██║ ╚████║██████╔╝██║██║ ╚████║╚██████╔╝
    // ╚═╝     ╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝╚═╝     ╚═╝╚═╝  ╚═══╝╚═════╝ ╚═╝╚═╝  ╚═══╝ ╚═════╝

    bool seen[60][60];
    unsigned int cost_m[60][60];

    // This data structure holds the result of a pathfinding call

    struct PathInfo {
        Direction dir; // Direction where you need to go
        number dist; // Distance to get to the destination
        Pos dest; // not useful

        PathInfo(D dir, number dist, Pos dest) : dir(dir), dist(dist), dest(dest) {}
        PathInfo() : dist(USHRT_MAX) {}

        bool found() { // Returns whether a path was found
            return dist != USHRT_MAX;
        }

        friend ostream& operator<<(ostream& o, const PathInfo& p) {
            o << "[ " << p.dist << ", " << p.dest << ", " << p.dir << " ]";

            return o;
        }
    };

    struct BFSPathInfo: public PathInfo {
        number dist_next_left; // Rounds left to get to the next node
        number dist_next;
        number dist_rounds;

        BFSPathInfo(D dir, number dist, number dist_next, number dist_rounds, Pos dest) : PathInfo(dir, dist, dest), dist_next_left(dist_next), dist_next(dist_next), dist_rounds(dist_rounds) {}
        BFSPathInfo() : PathInfo() {}

        friend ostream& operator<<(ostream& o, const BFSPathInfo& p) {
            o << "[ " << p.dist << ", " << p.dest << ", " << p.dir << ", " << p.dist_next_left << ", " << p.dist_rounds << " ]";

            return o;
        }
    };

    // This should never be called
    BFSPathInfo bfs(
            P initial,
            const function<bool(P)>& can_go,
            const function<bool(P)>& is_dest,
            const function<number(P, number)>& dist,
            const function<void(BFSPathInfo&)>& visit = [](BFSPathInfo& p) {},
            number max_dist = USHRT_MAX,
            bool first_allowed = true)
    {
        if (is_dest(initial) and first_allowed) return { {0,0}, 0, 0, 0, initial };

        queue<BFSPathInfo> queue;
        memset(seen, 0, sizeof seen); seen[initial.i][initial.j] = true;

        number dist_first = dist(initial, 0);

        for (D dir : dirs) {
            Pos dest = initial + dir;

            if (can_go(dest)) {
                seen[dest.i][dest.j] = true;
                number dist_initial_left = dist(dest, 1);
                queue.push(BFSPathInfo(dir, 1, dist_initial_left, dist_first, dest));
            }
        }

        while (not queue.empty() and not is_dest(queue.front().dest)) {
            BFSPathInfo pathInfoOrigin = queue.front();
            queue.pop();

            if (--pathInfoOrigin.dist_next_left > 0) {
                queue.push(pathInfoOrigin);
                continue;
            }

            visit(pathInfoOrigin);


            for (D dir : dirs) {
                Pos dest = pathInfoOrigin.dest + dir;

                if (can_go(dest) and not seen[dest.i][dest.j]) {
                    seen[dest.i][dest.j] = true;

                    number dist_next = dist(dest, pathInfoOrigin.dist + 1);
                    number total_dist_rounds = pathInfoOrigin.dist_rounds + pathInfoOrigin.dist_next;

                    if (total_dist_rounds <= max_dist) {
                        queue.push(BFSPathInfo(pathInfoOrigin.dir, pathInfoOrigin.dist + 1, dist_next, total_dist_rounds, dest));
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
            const function<bool(P)>& is_dest)
    {
        if (unit.type == Warrior) return bfs(unit.pos, can_go, is_dest, [](P pos, number distance_travelled) { return 1; });
        else return
                    bfs(
                            unit.pos,
                            can_go,
                            is_dest,
                            [&unit, this](P pos, number distance_travelled) {
                                return rounds_to_move_car(unit, pos, distance_travelled);
                            }
                    );
    }

    PathInfo bfs(
            const Unit& unit,
            const function<bool(P)>& is_dest,
            Safety safety,
            number max_dist = USHRT_MAX,
            bool first_allowed = true)
    {
        return bfs(unit, [&unit, this, safety](P pos) { return can_move_to(unit, pos, safety); }, is_dest);
    }


    PathInfo bfs(const Unit& unit, P dest, Safety safety) {
        return bfs(
                unit,
                [&unit, this, safety](P pos) { return can_move_to(unit, pos, safety); },
                [&dest](P pos) { return dest == pos; }
        );
    }

    PathInfo bfs(
            const Unit& unit,
            Safety safety,
            Owner owner,
            UnitType type = UnitTypeSize)
    {
        return bfs(
                unit,
                [&unit, this, safety](P pos) { return can_move_to(unit, pos, safety); },
                [owner, type, this](P pos) { return is_on(pos, owner, type); }

        );
    }

    PathInfo bfs(const Unit& unit, Safety safety, Adjacency adjacency, CellType type) {
        function<bool(P)> is_dest;

        if (adjacency == Adjacent) is_dest = [type, this](P pos) { return is_adjacent_to(pos, type); };
        else is_dest = [type, this](P pos) { return is(pos, type); };


        return bfs(
                unit,
                [&unit, this, safety](P pos) { return can_move_to(unit, pos, safety); },
                is_dest
        );
    }

    PathInfo bfs(const Unit& unit, Safety safety, Adjacency adjacency, Owner owner) {
        function<bool(P)> is_dest;

        if (adjacency == Adjacent) is_dest = [owner, this](P pos) { return is_adjacent_to(pos, owner); };
        else is_dest = [owner, this](P pos) { return is(pos, owner); };


        return bfs(
                unit,
                [&unit, this, safety](P pos) { return can_move_to(unit, pos, safety); },
                is_dest
        );
    }

    PathInfo bfs(const Unit& unit, Safety safety, Resource resource) {
        function<bool(P)> is_dest;

        if (resource == Drink) is_dest = [this](P pos) { return cinfo[pos].gives_water; };
        else is_dest = [this](P pos) { return cinfo[pos].gives_fuel; };


        return bfs(
                unit,
                [&unit, this, safety](P pos) { return can_move_to(unit, pos, safety); },
                is_dest
        );
    }


    struct WSPPathInfo: public PathInfo {
        unsigned int cost;

        WSPPathInfo(D dir, number dist, unsigned int cost, Pos dest) : PathInfo(dir, dist, dest), cost(cost) {}
        WSPPathInfo() : PathInfo() {}

        bool operator>(const WSPPathInfo& other) const {
            return cost > other.cost;
        }
    };

    WSPPathInfo wsp(
            P initial,
            const function<bool(P)>& can_go,
            const function<bool(P)>& is_dest,
            const function<number(P, number)>& dist,
            const function<number(P)>& cost,
            number max_dist = USHRT_MAX,
            bool first_allowed = false)
    {
        if (is_dest(initial) and first_allowed) return { {0,0}, 0, 0, initial };

        priority_queue<WSPPathInfo, vector<WSPPathInfo>, greater<WSPPathInfo>> queue;

        memset(seen, 0, sizeof seen); seen[initial.i][initial.j] = true;
        memset(cost_m, INT_MAX, sizeof cost_m);

        number dist_initial_left = dist(initial, 0);
        for (D dir : dirs) {
            Pos dest = initial + dir;

            if (can_go(dest)) {
                cost_m[dest.i][dest.j] = cost(dest);
                queue.push(WSPPathInfo(dir, dist_initial_left, cost_m[dest.i][dest.j], dest));
            }
        }

        while (not queue.empty() and not is_dest(queue.top().dest)) {
            WSPPathInfo pathInfoOrigin = queue.top();
            queue.pop();

            P from = pathInfoOrigin.dest;

            if (not seen[from.i][from.j]) {
                seen[from.i][from.j] = true;

                for (D dir : dirs) {
                    Pos dest = pathInfoOrigin.dest + dir;

                    unsigned int cost_dest = cost(dest);
                    unsigned int new_cost = cost_m[from.i][from.j] + cost_dest;

                    if (can_go(dest) and new_cost < cost_m[dest.i][dest.j]) {
                        cost_m[dest.i][dest.j] = new_cost;
                        seen[dest.i][dest.j] = true;

                        number dist_next = dist(pathInfoOrigin.dest, pathInfoOrigin.dist);
                        number total_dist = pathInfoOrigin.dist + dist_next;

                        if (total_dist <= max_dist) {
                            queue.push(WSPPathInfo(pathInfoOrigin.dir, total_dist, new_cost, dest));
                        }
                    }
                }
            }
        }

        if (not queue.empty()) return queue.top();
        else return {};
    }

    // endregion


    // region state
    // ███████╗████████╗ █████╗ ████████╗███████╗
    // ██╔════╝╚══██╔══╝██╔══██╗╚══██╔══╝██╔════╝
    // ███████╗   ██║   ███████║   ██║   █████╗
    // ╚════██║   ██║   ██╔══██║   ██║   ██╔══╝
    // ███████║   ██║   ██║  ██║   ██║   ███████╗
    // ╚══════╝   ╚═╝   ╚═╝  ╚═╝   ╚═╝   ╚══════╝

    typedef vector<int> VI;
    typedef vector<bool> VB;
    typedef vector<VB> VVB;

    vector<int> my_car_ids;
    vector<int> my_warrior_ids;

    CellType cell_type[60][60];


    struct CityInfo {
        vector<Pos> pos;

        snumber own_units;
        snumber enemy_units;
        snumber own_water;
        snumber enemy_water;

        snumber owner;

        float avg_water_distance;
    };

    CityInfo city_info[8];


    struct CellInfo {
        snumber possible_movements = 0;

        snumber enemy_warriors = 0;
        snumber enemy_warriors_water = 0;
        snumber enemy_warriors_life = 0;
        snumber enemy_cars = 0;
        number enemy_cars_dist = 0;

        snumber own_warriors = 0;
        snumber own_warriors_water = 0;
        snumber own_warriors_life = 0;
        snumber own_cars = 0;

        snumber city_id = 8;

        bool gives_water = false;
        bool gives_fuel = false;


        PathInfo water;
        PathInfo city;
        PathInfo station;
        PathInfo road;

        PathInfo cities[8];

        //PathInfo to[60][60];

        /*PathInfo& operator[](P pos) {
            return to[pos.i][pos.j];
        }*/

        friend ostream& operator<<(ostream& os, const CellInfo& c) {
            os << "---- CellInfo -----" << endl
               << "possible_movements: " << int(c.possible_movements) << endl << endl
               << "enemy_warriors: " << int(c.enemy_warriors) << endl
               << "enemy_warriors_water: " << int(c.enemy_warriors_water) << endl
               << "enemy_warriors_life: " << int(c.enemy_warriors_life) << endl
               << "enemy_cars: " << int(c.enemy_cars) << endl << endl
               << "city id: " << int(c.city_id) << endl << endl
               << (c.gives_fuel ? "gives fuel" : c.gives_water ? "gives_water" : "does not give") << endl << endl
               << "water: " << c.water << endl
               << "station: " << c.station << endl
               << "city: " << c.city << endl
               << "road: " << c.road << endl << endl;


            for (int i = 0; i < 8; ++i) {
                os << "city " << i << ": " << c.cities[i] << endl;
            }

            os << "-------------------" << endl;

            return os;
        }
    };

    CellInfo& operator[](P pos) {
        return cell_info[pos.i][pos.j];
    }


    CellInfo cell_info[60][60];

    // endregion


    // region strategy
    // ███████╗████████╗██████╗  █████╗ ████████╗███████╗ ██████╗██╗   ██╗
    // ██╔════╝╚══██╔══╝██╔══██╗██╔══██╗╚══██╔══╝██╔════╝██╔════╝╚██╗ ██╔╝
    // ███████╗   ██║   ██████╔╝███████║   ██║   █████╗  ██║  ███╗╚████╔╝
    // ╚════██║   ██║   ██╔══██╗██╔══██║   ██║   ██╔══╝  ██║   ██║ ╚██╔╝
    // ███████║   ██║   ██║  ██║██║  ██║   ██║   ███████╗╚██████╔╝  ██║
    // ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚══════╝ ╚═════╝   ╚═╝

    int score_dir_warrior(const Unit& warrior, D dir, CellType type) {
        P dest = warrior.pos + dir;

        const CellInfo& c = cinfo[dest];

        number dist = type == Water ? c.water.dist : c.city.dist;

        bool car_in_pos = is_on(dest, Anyone, Car);
        bool own_unit_in_pos = is_on(dest, Own);
        bool enemy_unit_in_pos = is_on(dest, Enemy, Warrior);

        return -car_in_pos*100000000 -own_unit_in_pos*10000000 -enemy_unit_in_pos*3000000 -c.enemy_cars_dist*1000000 - c.enemy_warriors*100000 - dist*1000 + c.possible_movements*100 + c.gives_water*10;
    }

    Direction choose_best_dir_warrior(const Unit& warrior, CellType type) {
        // TODO: Implement sort on copied dirs array (including None position) with comparison function, choose first available

        int max_score = INT_MIN;
        Direction best_dir = { 0, 0 };

        for (D dir : c_dirs_all) {
            if (can_move_to_simple_warrior(warrior.pos + dir)) {
                int score = score_dir_warrior(warrior, dir, type);

                if (score > max_score) {
                    max_score = score;
                    best_dir = dir;
                }
            }
        }

        //cerr << max_score << endl;

        return best_dir;
    }

    bool is_safe_warrior(P pos) {
        return cinfo[pos].enemy_warriors == 0 and cinfo[pos].enemy_cars == 0;
    }

    // Assumes that warrior is currently safe
    inline bool should_go_for_water(const Unit& warrior) {
        const CellInfo& info = cinfo[warrior.pos];

        P step = warrior.pos + info.water.dir;
        P dest = info.water.dest;

        bool should_go_passby =
                (
                        (warrior.water < 30 and info.water.dist <= 4) or
                        (warrior.water < 25 and info.water.dist <= 6) or
                        (warrior.water < 20 and info.water.dist <= 9)
                )
                and is_safe_warrior(dest)
                and is_safe_warrior(step);

        if (should_go_passby) return true;
        else {
            return warrior.water - info.water.dist <= 8;
        }
    }

    void handle_warrior_escape_from_car(const Unit& warrior) {
        action(warrior, choose_best_dir_warrior(warrior, City), unit_life(warrior)*1000);
    }

    void handle_warrior_threat(const Unit& warrior) {
        float max_probability = 0;
        Direction max_probability_dir;
        bool found_thunderdome = false;

        for (D dir : dirs) {
            P dest = warrior.pos + dir;

            if (is_on(dest, Enemy, Warrior)) {
                if (is_thunderdome(warrior.pos, dest) and can_demand(warrior, dest)) {
                    float prob = win_probability_warrior_thunderdome(warrior, unit(cell(dest).id));

                    if (prob > max_probability) {
                        max_probability = prob;
                        max_probability_dir = dir;
                        found_thunderdome = true;
                    }
                }
            }
        }

        if (found_thunderdome) {
            if (max_probability >= 0.55) {
                //cerr << "warrior " << warrior.pos << " will fight thunderdome" << endl;
                return action(warrior, max_probability_dir, warrior.water*10);
            }
            else {
                //cerr << "warrior " << warrior.pos << " fleeing from thunderdome" << endl;
                return action(warrior, choose_best_dir_warrior(warrior, City), unit_life(warrior)*100);
            }
        }
        else {
            // TODO: If warrior is on a city, and the enemy is on the outside, and has less water, let him in

            // TODO: See if the situation is favorable based on
            //        snumber enemy_warriors = 0;
            //        snumber enemy_warriors_water = 0;
            //        snumber enemy_warriors_life = 0;
            //        snumber enemy_cars = 0;
            //
            //        snumber own_warriors = 0;
            //        snumber own_warriors_water = 0;
            //        snumber own_warriors_life = 0;
            //        snumber own_cars = 0;
            // if favorable, attack

            // TODO: Simulate current situation
            if (unit_life(warrior) <= 6*cinfo[warrior.pos].enemy_warriors + 3) {
                number total_own_attack = 0;

                for (D dir : dirs) {
                    P dest = warrior.pos + dir;
                    if (is_on(dest, Enemy, Warrior)) {
                        total_own_attack += cinfo[dest].own_warriors*6;
                    }
                }

                if (cinfo[warrior.pos].enemy_warriors_life > total_own_attack) {
                    //cerr << "warrior " << warrior.pos << " fleeing from warrior threat because he's about to die" << endl;
                    return action(warrior, choose_best_dir_warrior(warrior, City), unit_life(warrior)*100);
                }
            }

            bool found = false;
            number max_score = 0;
            number max_own_units = 0;
            Direction best_dir;
            for (D dir : dirs) {
                P dest = warrior.pos + dir;

                if (can_demand(warrior, dest) and is_on(dest, Enemy, Warrior)) {
                    number own_units = cinfo[dest].own_warriors;
                    number score = own_units*6 - unit_life(unit_pos(dest));

                    if (score > max_score) {
                        found = true;
                        max_score = score;
                        best_dir = dir;
                        max_own_units = own_units;
                    }
                }
            }

            if (found and max_own_units < cinfo[warrior.pos].enemy_warriors) {
                //cerr << "warrior " << warrior.pos << " fleeing from warrior threat because its not favorable" << endl;
                return action(warrior, choose_best_dir_warrior(warrior, City), unit_life(warrior)*100);
            }
            else {
                //cerr << "warrior" << warrior.pos << " attacking other warrior" << endl;
                return action(warrior, best_dir, 1);
            }

            // Attack the one with best own_warrior_life - enemy_warrior_life
            // If the current warrior is about to die and all enemy warriors cannot be killed in this round, fleet. take in account that food is stolen at every attack
            // Else if there is no position such that own_warrior_water > enemy_warrior_water, fleet
        }
    }

    void handle_warrior_goto_city(const Unit& warrior) {
        if (is(warrior.pos, City)) {
            /*for (P pos : city_info[cinfo[warrior.pos].city_id].pos) {
                // TODO: Implement position demands system
            }*/

            // TODO: Go on positions which have soldiers at close distance
            //cerr << "Warrior " << warrior.pos << " stay in city" << endl;
            action(warrior, None, 50000);
        }
        else {
            //cerr << "Warrior " << warrior.pos << " going to city" << endl;
            action(warrior, choose_best_dir_warrior(warrior, City), 1);
            // Not in a city
            // TODO: Decide where to go
            // TODO: Decision should stick
        }
    }


    void compute_action_warrior(const Unit& warrior) {
        //cerr << "Warrior " << warrior.id << ": " << warrior.food << ' ' << warrior.water << endl;
        const CellInfo& info = cinfo[warrior.pos];

        if (info.enemy_cars > 0) {
            //cerr << "escape " << warrior.pos << " from car bro!" << endl;
            handle_warrior_escape_from_car(warrior);
        }
        else if (info.enemy_warriors > 0) {
            handle_warrior_threat(warrior);
        }
        else {
            // Not in danger bro

            // Look for water
            if (should_go_for_water(warrior)) {
                //cerr << "warrior " << warrior.pos << " going for water" << endl;
                action(warrior, choose_best_dir_warrior(warrior, Water), 40 - warrior.water);
            }
            else { // Go to city
                handle_warrior_goto_city(warrior);
            }
        }
    }


    void compute_action_car(const Unit& car) {
        if (not can_move(car.id)) return set_inactive_round(car);

        // TODO: Do BFS that ends when the n_units_not_already_targeted_outside_city/n_own_cars unit has been killed

        // Car demands all targets in its path
        // Next car does the same not targeting those already targeted
        // TODO: Ensure that cars are not very close together

        PathInfo station = bfs(car, Safe, Fuel);
        PathInfo enemy = bfs(car, Safe, Enemy, Warrior);

        if (station.found()) {
            if ((not enemy.found() or enemy.dist > 4) and car.food < 85 and station.dist < 4) {
                //cerr << "car " << car.pos << "going for station because it's near" << endl;
                return action(car, station.dir, 2);
            }
            else if ((not enemy.found() or enemy.dist > 1) and car.food - station.dist <= 0) {
                //cerr << "car " << car.pos << " going for station because it's out of gas " << station.dir << ' ' << car.food << ' ' << station.dist << endl;
                return action(car, station.dir, 2);
            }
        }


        if (enemy.found()) {
            //cerr << "car " << car.pos << " attacking " << enemy.dir << ' ' << enemy.dist << endl;
            return action(car, enemy.dir, 100000);
        }

        PathInfo enemy_city = bfs(car, Safe, Adjacent, Enemy);

        if (enemy_city.found()) {
            return action(car, enemy_city.dir, 1);
        }

        //cerr << "car " << car.pos << "doing nothing" << endl;

        return action(car, None, 600000);
    }


    void compute_actions() {
        list<number> units_list(units_to_command.begin(), units_to_command.end());

        for (number unit_id : units_list) {
            Unit m_unit = unit(unit_id);
            if (m_unit.type == Warrior) compute_action_warrior(m_unit);
            else compute_action_car(m_unit);
        }
    }

    // endregion


    // region round-init
    // ██████╗  ██████╗ ██╗   ██╗███╗   ██╗██████╗     ██╗███╗   ██╗██╗████████╗
    // ██╔══██╗██╔═══██╗██║   ██║████╗  ██║██╔══██╗    ██║████╗  ██║██║╚══██╔══╝
    // ██████╔╝██║   ██║██║   ██║██╔██╗ ██║██║  ██║    ██║██╔██╗ ██║██║   ██║
    // ██╔══██╗██║   ██║██║   ██║██║╚██╗██║██║  ██║    ██║██║╚██╗██║██║   ██║
    // ██║  ██║╚██████╔╝╚██████╔╝██║ ╚████║██████╔╝    ██║██║ ╚████║██║   ██║
    // ╚═╝  ╚═╝ ╚═════╝  ╚═════╝ ╚═╝  ╚═══╝╚═════╝     ╚═╝╚═╝  ╚═══╝╚═╝   ╚═╝

    // Shuffles the directions array
    // I don't remember why but this produces better warrior/car movements overall
    void shuffle_dirs() {
        number i = 0;
        VI permutation = random_permutation(8);

        for (Direction& dir : dirs) dir = c_dirs[permutation[i++]];
    }


    void get_my_units() {
        my_car_ids = cars(me());
        my_warrior_ids = warriors(me());
    }

    struct CarPreBFSPathInfo: public BFSPathInfo {
        bool station_in_path;

        CarPreBFSPathInfo(number dist, number dist_rounds, Pos dest, bool station_in_path) : BFSPathInfo({0,0}, dist, 0, dist_rounds, dest), station_in_path(station_in_path) {}
        CarPreBFSPathInfo() : BFSPathInfo() {}
    };

    void init_unitinfo() {
        for (int i = 0; i < rows(); ++i) {
            for (int j = 0; j < cols(); ++j) {
                Pos pos(i, j);

                cinfo[pos].enemy_warriors = 0;
                cinfo[pos].enemy_warriors_water = 0;
                cinfo[pos].enemy_warriors_life = 0;
                cinfo[pos].enemy_cars = 0;
                cinfo[pos].enemy_cars_dist = 0;
                cinfo[pos].own_cars = 0;
                cinfo[pos].own_warriors = 0;
                cinfo[pos].own_warriors_life = 0;
                cinfo[pos].own_warriors_water = 0;
            }
        }

        for (snumber player_id = 0; player_id < 4; ++player_id) {
            if (player_id == me()) {
                for (int unit_id : warriors(player_id)) {
                    const Unit& u = unit(unit_id);

                    for (D dir : dirs) {
                        P dest = u.pos + dir;

                        if (can_move_to_simple_warrior(dest)) {
                            ++cinfo[dest].own_warriors;
                            cinfo[dest].own_warriors_water += u.water;
                            cinfo[dest].own_warriors_life += unit_life(u);
                        }
                    }
                }

                for (int unit_id : cars(player_id)) {
                    const Unit& u = unit(unit_id);

                    P initial = u.pos;

                    bfs(
                            initial,
                            [this](P pos) { return can_move_to_simple_car(pos); },
                            [](P pos) { return false; },
                            [this, &u](P pos, number dist) { return rounds_to_move_car(u, pos, dist); },
                            [this](const BFSPathInfo& pathInfo) {
                                ++cinfo[pathInfo.dest].own_cars;
                            },
                            4
                    );
                }
            }
            else {
                for (int unit_id : warriors(player_id)) {
                    const Unit& u = unit(unit_id);

                    for (D dir : dirs) {
                        P dest = u.pos + dir;

                        if (can_move_to_simple_warrior(dest)) {
                            ++cinfo[dest].enemy_warriors;
                            cinfo[dest].enemy_warriors_water += u.water;
                            cinfo[dest].enemy_warriors_life += unit_life(u);
                        }
                    }
                }

                for (int unit_id : cars(player_id)) {
                    const Unit& u = unit(unit_id);

                    P initial = u.pos;

                    bfs(
                            initial,
                            [this](P pos) { return can_move_to_simple_car(pos); },
                            [](P pos) { return false; },
                            [this, &u](P pos, number dist) { return rounds_to_move_car(u, pos, dist); },
                            [this](const BFSPathInfo& pathInfo) {
                                if (pathInfo.dist_rounds <= 4) ++cinfo[pathInfo.dest].enemy_cars;
                                cinfo[pathInfo.dest].enemy_cars_dist += 8 - pathInfo.dist_rounds;
                            },
                            8
                    );
                }
            }
        }


        for (CityInfo& cityInfo : city_info) {
            cityInfo.own_units = 0;
            cityInfo.enemy_units = 0;
            cityInfo.own_water = 0;
            cityInfo.enemy_water = 0;
            cityInfo.owner = cell(cityInfo.pos[0]).owner;

            for (P pos : cityInfo.pos) {
                if (is_on(pos, Own, Warrior)) {
                    ++cityInfo.own_units;
                    cityInfo.own_water += unit_pos(pos).water;
                }
                else if (is_on(pos, Enemy, Warrior)) {
                    ++cityInfo.enemy_units;
                    cityInfo.enemy_water += unit_pos(pos).water;
                }
            }
        }

    }

    void round_init() {
        get_my_units();

        used_positions.clear();

        if (can_warriors_move()) units_to_command.insert(my_warrior_ids.begin(), my_warrior_ids.end());
        units_to_command.insert(my_car_ids.begin(), my_car_ids.end());

        init_unitinfo();
    }

    // endregion


    // region first-init
    // ███████╗██╗██████╗ ███████╗████████╗    ██╗███╗   ██╗██╗████████╗
    // ██╔════╝██║██╔══██╗██╔════╝╚══██╔══╝    ██║████╗  ██║██║╚══██╔══╝
    // █████╗  ██║██████╔╝███████╗   ██║       ██║██╔██╗ ██║██║   ██║
    // ██╔══╝  ██║██╔══██╗╚════██║   ██║       ██║██║╚██╗██║██║   ██║
    // ██║     ██║██║  ██║███████║   ██║       ██║██║ ╚████║██║   ██║
    // ╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝   ╚═╝       ╚═╝╚═╝  ╚═══╝╚═╝   ╚═╝


    // Called only on the first round to perform some initialization

    void check_and_add_proximities(P orig, P dest, const PathInfo& to) {
        CellInfo& cellinfo = cinfo[orig];

        if (cinfo[dest].gives_water) {
            if (not cellinfo.water.found()) cellinfo.water = to;
        }

        if (cinfo[dest].gives_fuel) {
            if (not cellinfo.station.found()) cellinfo.station = to;
        }

        if (is(dest, City)) {
            if (not cellinfo.city.found()) cellinfo.city = to;

            if (not cellinfo.cities[cinfo[dest].city_id].found()) {
                cellinfo.cities[cinfo[dest].city_id] = to;
            }
        }
        else if (is(dest, Road)) {
            if (not cellinfo.road.found()) cellinfo.road = to;
        }
    }

    void init_cellinfo() {
        for (number i = 0; i < rows(); ++i) {
            for (number j = 0; j < cols(); ++j) {
                memset(seen, 0, sizeof seen);

                P initial = Pos(i, j);
                queue<PathInfo> queue;

                check_and_add_proximities(initial, initial, PathInfo({0,0}, 0, initial));

                for (D dir : dirs) {
                    P dest = initial + dir;

                    if (can_move_to_simple_warrior(dest)) {
                        ++cinfo[initial].possible_movements;

                        seen[dest.i][dest.j] = true;
                        PathInfo to(dir, 1, dest);
                        queue.push(to);

                        check_and_add_proximities(initial, dest, to);
                    }
                }

                while (not queue.empty()) {
                    PathInfo pathInfoOrigin = queue.front();
                    queue.pop();

                    for (D dir : dirs) {
                        Pos dest = pathInfoOrigin.dest + dir;

                        if (can_move_to_simple_warrior(dest) and not seen[dest.i][dest.j]) {
                            seen[dest.i][dest.j] = true;

                            PathInfo to(pathInfoOrigin.dir, pathInfoOrigin.dist + 1, dest);

                            queue.push(to);

                            check_and_add_proximities(initial, dest, to);
                        }
                    }
                }
            }
        }


        for (CityInfo& cityInfo : city_info) {
            number total = 0;
            for (P pos : cityInfo.pos) {
                total += cinfo[pos].water.dist;
            }

            cityInfo.avg_water_distance = float(total)/cityInfo.pos.size();

            sort(cityInfo.pos.begin(), cityInfo.pos.end(), [this](const Pos& a, const Pos& b) { return cinfo[a].water.dist  < cinfo[b].water.dist; });
        }
    }

    void init_cityinfo() {
        memset(seen, false, sizeof seen);
        queue<Pos> queue;

        int id = 0;

        for (number i = 0; i < rows(); ++i) {
            for (number j = 0; j < rows(); ++j) {
                if (not seen[i][j]) {
                    Pos initial(i, j);

                    seen[i][j] = true;

                    if (not is(initial, City)) {
                        cinfo[initial].city_id = nb_cities();
                    }
                    else {
                        queue.push(initial);

                        city_info[id].pos = vector<Pos>();

                        while (not queue.empty()) {
                            P from = queue.front();
                            queue.pop();

                            cinfo[from].city_id = id;
                            city_info[id].pos.push_back(from);


                            for (D dir : dirs) {
                                P dest = from + dir;

                                if (not seen[dest.i][dest.j]) {
                                    seen[dest.i][dest.j] = true;

                                    if (is(dest, City)) queue.push(dest);
                                    else cinfo[dest].city_id = nb_cities();
                                }
                            }
                        }

                        ++id;
                    }
                }
            }
        }
    }

    void init_celltype() {
        for (number i = 0; i < rows(); ++i) {
            for (number j = 0; j < cols(); ++j) {
                cell_type[i][j] = cell(i, j).type;

                if (cell_type[i][j] == Water) {
                    for (D dir : dirs) {
                        P dest = Pos(i, j) + dir;
                        if (can_move_to_simple_warrior(dest)) {
                            cinfo[dest].gives_water = true;
                        }
                    }
                }
                else if (cell_type[i][j] == Station) {
                    for (D dir : dirs) {
                        P dest = Pos(i, j) + dir;
                        if (can_move_to_simple_warrior(dest)) {
                            cinfo[dest].gives_fuel = true;
                        }
                    }
                }
            }
        }
    }

    void first_init() {
        init_celltype();
        init_cityinfo();
        init_cellinfo();
    }

    // endregion


    // region play
    // ██████╗ ██╗      █████╗ ██╗   ██╗
    // ██╔══██╗██║     ██╔══██╗╚██╗ ██╔╝
    // ██████╔╝██║     ███████║ ╚████╔╝
    // ██╔═══╝ ██║     ██╔══██║  ╚██╔╝
    // ██║     ███████╗██║  ██║   ██║
    // ╚═╝     ╚══════╝╚═╝  ╚═╝   ╚═╝

    void play() {
        shuffle_dirs();

        if (round() == 0) first_init();

        round_init();

        int commanded = 0;
        do {
            int previous_units_to_command = units_to_command.size();
            compute_actions();
            process_actions();
            commanded = previous_units_to_command - units_to_command.size();
        } while(not units_to_command.empty() and commanded > 0);


        //ensure(units_to_command.empty(), "Some units not commanded!")
        units_to_command.clear();
    }

    // endregion
};

RegisterPlayer(PLAYER_NAME);