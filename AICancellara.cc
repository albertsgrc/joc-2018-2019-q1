#include <cassert>

#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Cancellara

#define ensure(condition, message) { if (not (condition)) { cerr << "ERROR: " << (message) << endl; assert(condition); } }

// DISCLAIMER: The following Demo player is *not* meant to do anything
// sensible. It is provided just to illustrate how to use the API.
// Please use AINull.cc as a template for your player.

// Returns a position identifier
inline int operator~(const Pos& pos) {
    return pos.i*60 + pos.j;
}

struct PLAYER_NAME : public Player {

    /**
     * Factory: returns a new instance of this class.
     * Do not modify this function.
     */
    static Player *factory() {
        return new PLAYER_NAME;
    }

    //
    const Dir c_dirs[8] = {
        Bottom, BR, Right, RT, Top, TL, Left, LB
    };

    Dir dirs[8];


    // Helper methods

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



    // Movement helpers

    inline bool can_warriors_move() {
        return round() % 4 != me();
    }

    inline bool can_move_to(int id, const Pos& pos) {
        return can_move_to(unit(id), pos);
    }

    // Assumes round()%4 == me()
    // Assumes pos is adjacent to id
    inline bool can_move_to(const Unit& unit, const Pos& pos) {
        ensure(is_adjacent_to(unit, pos), "can_move_to with non-adjacent position");
        ensure(unit.type == Car or can_warriors_move(), "can_move_to for warrior on invalid round");

        if (not pos_ok(pos)) return false;

        if (in(used_positions, ~pos)) return false;

        Cell c = cell(pos.i, pos.j);
        switch (unit.type) {
            case Warrior: return one_of(c.type, Desert, City, Road);
            case Car: return can_move(unit.id) and one_of(c.type, Desert, Road);
            default: _unreachable();
        }
    }

    inline bool is_adjacent_to(const Unit& unit, const Pos& pos) {
        return is_adjacent_to(unit.pos, pos);
    }

    inline bool is_adjacent_to(const Unit& unit, CellType t) {
        return is_adjacent_to(unit.pos, t);
    }

    inline bool is_adjacent_to(const Unit& unit, int other_unit_id) {
        return is_adjacent_to(unit.pos, other_unit_id);
    }

    inline bool is_adjacent_to(const Unit& unit, UnitType type) {
        return is_adjacent_to(unit.pos, type);
    }

    inline bool is_adjacent_to(const Pos& pos, std::function<bool(const Pos&)> evaluator) {
        for (const Dir& d : dirs) {
            if (evaluator(pos + d)) return true;
        }
        return false;
    }

    inline bool is_adjacent_to(const Pos& pos, const Pos& other_pos) {
        return is_adjacent_to(pos, [&other_pos](const Pos& p) { return p == other_pos; });
    }

    inline bool is_adjacent_to(const Pos& pos, CellType type) {
        return is_adjacent_to(pos, [type, this](const Pos& p) { return cell(p).type == type; });
    }

    inline bool is_adjacent_to(const Pos& pos, int other_unit_id) {
        return is_adjacent_to(pos, [other_unit_id, this](const Pos& p) { return cell(p).id == other_unit_id; });
    }

    inline bool is_adjacent_to(const Pos& pos, UnitType type) {
        return is_adjacent_to(pos, [type, this](const Pos& p) {
            Cell c = cell(p);
            return c.id != -1 and unit(c.id).type == type;
        });
    }


    inline bool has_unit(const Pos& pos, UnitType type = UnitTypeSize) {
        if (type == UnitTypeSize) {
            return has_unit(pos, Warrior) or has_unit(pos, Car);
        } else {
            int unit_id = cell(pos).id;
            return cell(pos).id != -1 and unit(unit_id).type == type;
        }
    }

    inline bool is_on(int unit_id, CellType cellType) {
        return is_on(unit(unit_id), cellType);
    }


    inline bool is_on(const Unit& unit, CellType cellType) {
        return cell(unit.pos).type == cellType;
    }


    // Command control

    struct Action {
        int unit_id;
        Dir dir;
        float importance;

        Action(int unit_id, Dir dir, float importance) : unit_id(unit_id), dir(dir), importance(importance) {}

        bool operator<(const Action& other) const {
            return importance < other.importance;
        }
    };

    priority_queue<Action> actions_queue;
    set<int> units_to_command;
    set<int> used_positions;

    void action(int id, Dir dir, float importance) {
        actions_queue.push(Action(id, dir, importance));
    }

    void process_actions() {
        while (!actions_queue.empty()) {
            Action action = actions_queue.top(); actions_queue.pop();

            Pos destination = unit(action.unit_id).pos + action.dir;

            if (used_positions.insert(~destination).second) {
                // Was not a used position, perform command
                command(action.unit_id, action.dir);
                units_to_command.erase(action.unit_id);
            }
        }
    }


    typedef vector<int> VI;

    VI my_cars;
    VI my_warriors;


    void compute_actions() {
        // TODO: Implement
    }

    void shuffle_dirs() {
        int i = 0;
        VI permutation = random_permutation(8);

        for (Dir& dir : dirs) dir = c_dirs[permutation[i++]];
    }

    void first_round_initialization() {
        my_cars = cars(me());
        my_warriors = warriors(me());
    }


    void play() {
        if (round() == 0) first_round_initialization();

        shuffle_dirs();

        units_to_command.insert(my_warriors.begin(), my_warriors.end());
        units_to_command.insert(my_cars.begin(), my_cars.end());

        do {
            compute_actions();
            process_actions();
        } while(!units_to_command.empty());
    }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
