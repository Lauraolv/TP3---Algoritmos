#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

struct Stage {
    int bonus_multiplier;
    int max_time;
};

struct Trick {
    int time_required;
    int base_points;
};

class TrickScoringSystem {
private:
    vector<Stage> stages;
    vector<Trick> tricks;
    vector<vector<uint16_t>> combinations_per_stage;
    vector<vector<int64_t>> max_points_dp;
    vector<vector<int>> decisions; // Tracks the optimal decisions

    // Create all possible combinations of tricks
    void generateCombinations(int total_tricks) {
        int total_combinations = 1 << total_tricks;
        combinations_per_stage.resize(stages.size());

        for (int i = 0; i < total_combinations; ++i) {
            int cumulative_time = 0;
            for (int j = 0; j < total_tricks; ++j) {
                if (i & (1 << j)) {
                    cumulative_time += tricks[j].time_required;
                }
            }

            for (int s = 0; s < stages.size(); ++s) {
                if (cumulative_time <= stages[s].max_time) {
                    combinations_per_stage[s].push_back(i);
                }
            }
        }
    }

    // Calculate the potential points for a specific stage and trick combination
    int64_t calculateStagePoints(int stage_index, int trick_set, int prior_trick_set) {
        int64_t points = 0;
        int trick_count = __builtin_popcount(trick_set);

        for (int j = 0; j < tricks.size(); ++j) {
            if (trick_set & (1 << j)) {
                points += (prior_trick_set & (1 << j)) ? tricks[j].base_points / 2 : tricks[j].base_points;
            }
        }

        return points * stages[stage_index].bonus_multiplier * trick_count;
    }

    // Recursive function to determine the maximum score possible
    int64_t findMaxScore(int stage_index, int current_trick_set) {
        if (stage_index == stages.size()) return 0;
        if (max_points_dp[stage_index][current_trick_set] != numeric_limits<int64_t>::min()) {
            return max_points_dp[stage_index][current_trick_set];
        }

        int64_t max_points = 0;
        int optimal_trick_set = 0;

        for (uint16_t new_trick_set : combinations_per_stage[stage_index]) {
            int64_t potential_points = calculateStagePoints(stage_index, new_trick_set, current_trick_set);
            potential_points += findMaxScore(stage_index + 1, new_trick_set);

            if (potential_points > max_points) {
                max_points = potential_points;
                optimal_trick_set = new_trick_set;
            }
        }

        decisions[stage_index][current_trick_set] = optimal_trick_set; // Store the decision
        return max_points_dp[stage_index][current_trick_set] = max_points;
    }

public:
    TrickScoringSystem(const vector<Stage>& stages, const vector<Trick>& tricks)
        : stages(stages), tricks(tricks) {
        max_points_dp.resize(stages.size(), vector<int64_t>(1 << tricks.size(), numeric_limits<int64_t>::min()));
        decisions.resize(stages.size(), vector<int>(1 << tricks.size(), -1));
    }

    // Calculate the maximum score and print the results
    void computeAndOutputResults() {
        generateCombinations(tricks.size());
        int64_t max_score = findMaxScore(0, 0);

        // Output maximum score
        cout << max_score << endl;

        // Output the trick sets used in each stage
        int current_trick_set = 0;
        for (int i = 0; i < stages.size(); ++i) {
            int selected_trick_set = decisions[i][current_trick_set];
            vector<int> selected_tricks;

            for (int j = 0; j < tricks.size(); ++j) {
                if (selected_trick_set & (1 << j)) {
                    selected_tricks.push_back(j + 1); // Store the tricks used
                }
            }

            cout << selected_tricks.size();
            for (int trick : selected_tricks) {
                cout << " " << trick;
            }
            cout << endl;

            current_trick_set = selected_trick_set;
        }
    }
};

int main() {
    int num_stages, num_tricks;
    cin >> num_stages >> num_tricks;
    vector<Stage> stages(num_stages);
    vector<Trick> tricks(num_tricks);

    for (int i = 0; i < num_stages; ++i) {
        cin >> stages[i].bonus_multiplier >> stages[i].max_time;
    }
    for (int j = 0; j < num_tricks; ++j) {
        cin >> tricks[j].base_points >> tricks[j].time_required;
    }

    TrickScoringSystem scoring_system(stages, tricks);
    scoring_system.computeAndOutputResults();

    return 0;
}
