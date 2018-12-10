#include <cassert>
#include <list>
#include <climits>
#include <functional>
#include <cstring>
#include "Player.hh"

#define PLAYER_NAME Cancellara
#define IS_DEBUG 1

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

    // Assumes round()%4 == me()
    // Assumes pos is adjacent to id
    inline bool can_move_to(const Unit& unit, P pos, Safety safety, UseState useState = DoesntMatterTaken) {
        if (not pos_ok(pos)) return false;

        //ensure(is_adjacent_to(unit, pos), "can_move_to with non-adjacent position");
        ensure((unit.type == Car and can_move(unit.id)) or (unit.type == Warrior and can_warriors_move()), "can_move_to for warrior on invalid round");

        if (useState == NotTaken and is_taken(pos)) return false;

        if (is_on(pos, Own)) return false;

        if (unit.type == Car and (is_on(pos, Enemy, Car) or is_adjacent_to(pos, Enemy, Car))) return false;

        if (safety == Safe and is_adjacent_to(pos, Enemy)) return false;


        switch (unit.type) {
            case Warrior: return is_any(pos, { Desert, City, Road });
            case Car: return can_move(unit.id) and is_any(pos, { Desert, Road });
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

    inline bool is_taken(P pos) {
        return in(used_positions, ~pos);
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
    set<number> used_positions;

    // Call this method instead of command
    void action(snumber unit_id, Dir dir, float importance) {
        ensure(dir_ok(dir), "try to add action with invalid dir");
        actions_queue.push(Action(unit_id, dir, importance));
    }

    void action(const Unit& unit, Dir dir, float importance) {
        action(unit.id, dir, importance);
    }

    void action(const Unit& unit, const PathInfo& info, float importance) {
        action(unit.id, Dir(info.dir), importance);
    }

    void process_actions() {
        while (!actions_queue.empty()) {
            Action action = actions_queue.top(); actions_queue.pop();

            Pos destination = unit(action.unit_id).pos + action.dir;

            // TODO: Make the exception for positions where there is an enemy with enough life to not die even if all demands attack him
            if (used_positions.insert(~destination).second) {
                // Was not a used position, perform command
                ensure(cell(destination).id == -1 or cell(destination).id == action.unit_id or unit(cell(destination).id).player != me(), "attacking own unit!");

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
    };

    struct BFSPathInfo: public PathInfo {
        number dist_next_left; // Rounds left to get to the next node

        BFSPathInfo(D dir, number dist, number dist_next_left, Pos dest) : PathInfo(dir, dist, dest), dist_next_left(dist_next_left) {}
        BFSPathInfo() : PathInfo() {}
    };

    // This should never be called
    BFSPathInfo bfs(
            P initial,
            const function<bool(P)>& can_go,
            const function<bool(P)>& is_dest,
            const function<number(P, number)>& dist,
            number max_dist = USHRT_MAX,
            bool first_allowed = false)
    {
        if (is_dest(initial) and first_allowed) return { {0,0}, 0, 0, initial };

        queue<BFSPathInfo> queue;
        memset(seen, 0, sizeof seen); seen[initial.i][initial.j] = true;

        number dist_initial_left = dist(initial, 0);
        for (D dir : dirs) {
            Pos dest = initial + dir;

            if (can_go(dest) and not is_taken(dest)) {
                seen[dest.i][dest.j] = true;
                queue.push(BFSPathInfo(dir, dist_initial_left, dist_initial_left, dest));
            }
        }

        while (not queue.empty() and not is_dest(queue.front().dest)) {
            BFSPathInfo pathInfoOrigin = queue.front();
            queue.pop();

            if (--pathInfoOrigin.dist_next_left > 0) {
                queue.push(pathInfoOrigin);
                continue;
            }

            for (D dir : dirs) {
                Pos dest = pathInfoOrigin.dest + dir;

                if (can_go(dest) and not seen[dest.i][dest.j]) {
                    seen[dest.i][dest.j] = true;

                    number dist_next = dist(pathInfoOrigin.dest, pathInfoOrigin.dist);
                    number total_dist = pathInfoOrigin.dist + dist_next;

                    if (total_dist <= max_dist) {
                        queue.push(BFSPathInfo(pathInfoOrigin.dir, total_dist, dist_next, dest));
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
            number max_dist = USHRT_MAX,
            bool first_allowed = false)
    {
        if (unit.type == Warrior) return bfs(unit.pos, can_go, is_dest, [](P pos, number distance_travelled) { return 1; }, max_dist, first_allowed);
        else return
            bfs(
                unit.pos,
                can_go,
                is_dest,
                [&unit, this](P pos, number distance_travelled) {
                    return is(pos, Road) and (unit.food - distance_travelled > 0) ? 1 : 4;
                },
                max_dist,
                first_allowed
            );
    }

    PathInfo bfs(
            const Unit& unit,
            const function<bool(P)>& is_dest,
            Safety safety,
            number max_dist = USHRT_MAX,
            bool first_allowed = false)
    {
        return bfs(unit, [&unit, this, safety](P pos) { return can_move_to(unit, pos, safety); }, is_dest, max_dist, first_allowed);
    }


    PathInfo bfs(const Unit& unit, P dest, Safety safety, number maxdist = USHRT_MAX) {
        return bfs(
                unit,
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
            number max_dist = USHRT_MAX,
            bool first_allowed = false)
    {
        return bfs(
                unit,
                [&unit, this, safety](P pos) { return can_move_to(unit, pos, safety); },
                [owner, type, this](P pos) { return is_on(pos, owner, type); },
                max_dist,
                first_allowed
        );
    }

    PathInfo bfs(const Unit& unit, Safety safety, Adjacency adjacency, CellType type, number maxdist = USHRT_MAX) {
        function<bool(P)> is_dest;

        if (adjacency == Adjacent) is_dest = [type, this](P pos) { return is_adjacent_to(pos, type); };
        else is_dest = [type, this](P pos) { return is(pos, type); };


        return bfs(
                unit,
                [&unit, this, safety](P pos) { return can_move_to(unit, pos, safety); },
                is_dest,
                maxdist
        );
    }

    PathInfo bfs(const Unit& unit, Safety safety, Adjacency adjacency, Owner owner, number maxdist = USHRT_MAX) {
        function<bool(P)> is_dest;

        if (adjacency == Adjacent) is_dest = [owner, this](P pos) { return is_adjacent_to(pos, owner); };
        else is_dest = [owner, this](P pos) { return is(pos, owner); };


        return bfs(
                unit,
                [&unit, this, safety](P pos) { return can_move_to(unit, pos, safety); },
                is_dest,
                maxdist
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

            if (can_go(dest) and not is_taken(dest)) {
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


    // region strategy
    // ███████╗████████╗██████╗  █████╗ ████████╗███████╗ ██████╗██╗   ██╗
    // ██╔════╝╚══██╔══╝██╔══██╗██╔══██╗╚══██╔══╝██╔════╝██╔════╝╚██╗ ██╔╝
    // ███████╗   ██║   ██████╔╝███████║   ██║   █████╗  ██║  ███╗╚████╔╝
    // ╚════██║   ██║   ██╔══██╗██╔══██║   ██║   ██╔══╝  ██║   ██║ ╚██╔╝
    // ███████║   ██║   ██║  ██║██║  ██║   ██║   ███████╗╚██████╔╝  ██║
    // ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚══════╝ ╚═════╝   ╚═╝

    void compute_action_warrior(const Unit& warrior) {
        //cerr << "Warrior " << warrior.id << ": " << warrior.food << ' ' << warrior.water << endl;

        PathInfo enemy_warrior = bfs(warrior, Unsafe, Enemy, Warrior);

        if (enemy_warrior.found() and enemy_warrior.dist == 1 and unit(cell(enemy_warrior.dest).id).water <= warrior.water) {
            action(warrior, enemy_warrior, 30);
            return;
        }

        PathInfo water = bfs(warrior, Safe, Adjacent, Water);
        if (water.found() and water.dist >= warrior.water - 5) {
            //cerr << "Warrior going for wateeeer" << endl;
            action(warrior, water, 5);
            return;
        }

        PathInfo enemy_city = bfs(warrior, Unsafe, ExactlyThere, Enemy);
        if (enemy_city.found() and warrior.water > 25 and not is(warrior.pos, Own)) {
            //cerr << "Warrior going for enemy city" << endl;
            action(warrior, enemy_city, 1);
            return;
        }

        PathInfo city = bfs(warrior, Safe, ExactlyThere, Anyone);
        if (city.found() and city.dist >= warrior.food - 3) {
            action(warrior, city, 5);
            return;
        }

        PathInfo own_city = bfs(warrior, Safe, ExactlyThere, Own);
        if (own_city.found()) {
            action(warrior, own_city, 2);
            return;
        }

        if (water.found()) {
            action(warrior, water, 1);
            return;
        }

        action(warrior, None, 20);
    }


    void compute_action_car(const Unit& car) {
        if (not can_move(car.id)) return set_inactive_round(car);

        PathInfo station = bfs(car, Unsafe, Adjacent, Station);

        if (station.dist >= car.food) return action(car, station, 3);

        PathInfo enemy_warrior = bfs(car, Unsafe, Enemy, Warrior);

        if (enemy_warrior.found()) return action(car, enemy_warrior, 2);

        PathInfo enemy_city = bfs(car, Unsafe, Adjacent, Enemy);

        if (enemy_city.found()) return action(car, enemy_city, 1);

        PathInfo own_city = bfs(car, Unsafe, Adjacent, Own);

        if (own_city.found()) return action(car, own_city, 1);

        return action(car, station, 3);
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

    // endregion


    // region first-init
    // ███████╗██╗██████╗ ███████╗████████╗    ██╗███╗   ██╗██╗████████╗
    // ██╔════╝██║██╔══██╗██╔════╝╚══██╔══╝    ██║████╗  ██║██║╚══██╔══╝
    // █████╗  ██║██████╔╝███████╗   ██║       ██║██╔██╗ ██║██║   ██║
    // ██╔══╝  ██║██╔══██╗╚════██║   ██║       ██║██║╚██╗██║██║   ██║
    // ██║     ██║██║  ██║███████║   ██║       ██║██║ ╚████║██║   ██║
    // ╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝   ╚═╝       ╚═╝╚═╝  ╚═══╝╚═╝   ╚═╝

    CellType cell_type[60][60];


    struct CityInfo {
        vector<Pos> pos;
    };

    CityInfo city_info[8];


    struct CellInfo {
        snumber city_id;

        PathInfo water;
        PathInfo city;
        PathInfo station;
        PathInfo road;

        PathInfo cities[8];

        //PathInfo to[60][60];

        /*PathInfo& operator[](P pos) {
            return to[pos.i][pos.j];
        }*/
    };

    CellInfo& operator[](P pos) {
        return cell_info[pos.i][pos.j];
    }


    CellInfo cell_info[60][60];



    // Called only on the first round to perform some initialization

    void check_and_add_proximities(P orig, P dest, const PathInfo& to) {
        CellInfo& cellinfo = cinfo[orig];

        if (is(dest, Water)) {
            if (not cellinfo.water.found()) cellinfo.water = to;
        }
        else if (is(dest, City)) {
            if (not cellinfo.city.found()) cellinfo.city = to;

            if (not cellinfo.cities[cellinfo.city_id].found()) cellinfo.cities[cellinfo.city_id] = to;
        }
        else if (is(dest, Station)) {
            if (not cellinfo.station.found()) cellinfo.station = to;
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

                for (D dir : dirs) {
                    P dest = initial + dir;

                    if (can_move_to_simple_warrior(dest)) {
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
        for (number i = 0; i < rows(); ++i)
            for (number j = 0; j < cols(); ++j) cell_type[i][j] = cell(i, j).type;
    }

    void first_init() {
        init_celltype();
        //init_cityinfo();
        //init_cellinfo();
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

        get_my_units();

        used_positions.clear();

        if (can_warriors_move()) units_to_command.insert(my_warrior_ids.begin(), my_warrior_ids.end());
        units_to_command.insert(my_car_ids.begin(), my_car_ids.end());

        do {
            compute_actions();
            process_actions();
        } while(not units_to_command.empty());
    }

    // endregion
};

RegisterPlayer(PLAYER_NAME);
