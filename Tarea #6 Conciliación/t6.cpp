#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Result {
    int matches = 0;
    int total_diff = 0;
};

Result get_better_result(const Result& a, const Result& b) {
    if (a.matches > b.matches) {
        return a;
    }
    if (b.matches > a.matches) {
        return b;
    }
    if (a.total_diff < b.total_diff) {
        return a;
    }
    return b;
}

int main(){
   
    int Na, Nb, S;

    while (cin >> Na >> Nb >> S && (Na != 0 || Nb != 0 || S != 0)) {

        vector<int> timesA(Na);
        vector<int> timesB(Nb);

        for (int i = 0; i < Na; ++i) {
            int h, m, s;
            char colon;
            cin >> h >> colon >> m >> colon >> s;
            timesA[i] = h * 3600 + m * 60 + s;
        }

        for (int i = 0; i < Nb; ++i) {
            int h, m, s;
            char colon;
            cin >> h >> colon >> m >> colon >> s;
            timesB[i] = h * 3600 + m * 60 + s;
        }

        sort(timesA.begin(), timesA.end());
        sort(timesB.begin(), timesB.end());

        vector<vector<Result>> dp(Na + 1, vector<Result>(Nb + 1));

        for (int i = 1; i <= Na; ++i) {
            for (int j = 1; j <= Nb; ++j) {
                Result option1 = dp[i - 1][j];
                Result option2 = dp[i][j - 1];

                dp[i][j] = get_better_result(option1, option2);

                int time_diff = abs(timesA[i - 1] - timesB[j - 1]);

                if (time_diff <= S) {
                    Result match_option;
                    match_option.matches = dp[i - 1][j - 1].matches + 1;
                    match_option.total_diff = dp[i - 1][j - 1].total_diff + time_diff;
                    
                    dp[i][j] = get_better_result(dp[i][j], match_option);
                }
            }
        }

        Result final_result = dp[Na][Nb];

        if (final_result.matches == 0) {
            cout << "No matches" << endl;
        } else {
            double avg_diff = static_cast<double>(final_result.total_diff) / final_result.matches;
            cout << final_result.matches << " " << fixed << setprecision(1) << avg_diff << endl;
        }
    }

    return 0;
}