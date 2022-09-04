#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];


int pair_count;
int candidate_count;
int source;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
bool circle(int from, int to);
void print_winner();

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    //Set preferences to zeros
    for (int i = 0; i < MAX; i++)
    {
        for (int j = 0; j < MAX; j++)
        {
            preferences[i][j] = 0;
        }
    }

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }
    /*
        //print references
        for (int i = 0; i < candidate_count; i++)
        {
            for (int j = 0; j < candidate_count; j++)
            {
                printf("%i",preferences[i][j]);
            }
            printf("\n");
        }
    */

    add_pairs();
    /*
        //print add_pair
        for (int j = 0; j < 2; j++)
        {
            for (int i = 0; i < pair_count; i++)
            {
                if (j == 0 )
                {
                    printf("winner: %i      ",pairs[i].winner);
                }
                else
                {
                    printf("loser: %i       ",pairs[i].loser);
                }
            }
            printf("\n");
        }
    */

    sort_pairs();
    /*
        //print sort_pair
        for (int j = 0; j < 2; j++)
        {
            for (int i = 0; i < pair_count; i++)
            {
                if (j == 0 )
                {
                    printf("winner: %i      ",pairs[i].winner);
                }
                else
                {
                    printf("loser: %i       ",pairs[i].loser);
                }
            }
            printf("\n");
        }
    */
    lock_pairs();
    /*
        //print locked pairs
        for (int i = 0; i < candidate_count; i++)
        {
            for (int j = 0; j < candidate_count; j++)
            {
                printf("%d",locked[i][j]);
            }
            printf("\n");
        }

    */
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }

    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
            else if (preferences[i][j] < preferences[j][i])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    pair temp[1];
    int max = preferences[0][0];
    int max_pos = 0;    //max position
    for (int i = 0; i < pair_count; i++)
    {
        for (int j = i; j < pair_count; j++)
        {
            if (preferences[pairs[j].winner][pairs[j].loser] > max)
            {
                max = preferences[pairs[j].winner][pairs[j].loser];
                max_pos = j;
            }
        }
        if (max_pos != i)
        {
            // max_pos -> temp
            temp[0].winner = pairs[max_pos].winner;
            temp[0].loser = pairs[max_pos].loser;
            // i -> max_pos
            pairs[max_pos].winner = pairs[i].winner;
            pairs[max_pos].loser = pairs[i].loser;
            // temp -> i
            pairs[i].winner = temp[0].winner;
            pairs[i].loser = temp[0].loser;
        }
        max = preferences[0][0];

    }
    return;
}


// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        if (!circle(pairs[i].winner, pairs[i].loser))
        {
            // Lock the pair unless it makes a circle
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
//    printf("%i\n",source);
    return;
}

bool circle(int from, int to)
{
    if (to == from)
    {
        return true;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[to][i])
        {
            if (circle(from, i))
            {
                return true;
            }
        }
    }
    return false;
}

// Print the winner of the election
void print_winner()
{
    for (int i = 0; i < candidate_count; i++)
    {
        int count = 0;
        for (int j = 0; j < candidate_count; j++)
        {
            if (!locked[j][i])
            {
                count++;
            }
            if (count == candidate_count)
            {
                printf("%s\n", candidates[i]);
            }
        }
    }
}
