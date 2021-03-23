//Libraries
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <random>

//Macros
#define FACTOR_DOWN (100. / 1275.)
#define FACTOR_UP (1275. / 100.)
#define N_ENCOUNTERS 100000
#define OUTFILE "CatchRateTable.md"

//List of FireRed/LeafGreen safari pokemon
#define MAGIKARP Pokemon(25, 255)
#define NIDORANF Pokemon(50, 235)
#define NIDORANM Pokemon(50, 235)
#define PARAS Pokemon(50, 190)
#define VENONAT Pokemon(50, 190)
#define PSYDUCK Pokemon(50, 190)
#define POLIWAG Pokemon(50, 255)
#define SLOWPOKE Pokemon(50, 190)
#define DODUO Pokemon(50, 190)
#define GOLDEEN Pokemon(50, 225)
#define NIDORINO Pokemon(75, 120)
#define NIDORINA Pokemon(75, 120)
#define PARASECT Pokemon(75, 75)
#define VENOMOTH Pokemon(75, 75)
#define EXEGGCUTE Pokemon(75, 90)
#define RHYHORN Pokemon(75, 120)
#define SEAKING Pokemon(75, 60)
#define DRATINI Pokemon(100, 45)
#define CHANSEY Pokemon(125, 30)
#define KANGASKHAN Pokemon(125, 45)
#define SCYTHER Pokemon(125, 45)
#define PINSIR Pokemon(125, 45)
#define TAUROS Pokemon(125, 45)
#define DRAGONAIR Pokemon(125, 45)

//Loop to do on each pokemon
#define POKELOOP(pokemon)                                                                                        \
    for (uint i = 0; i < N_ENCOUNTERS; i++)                                                                      \
    {                                                                                                            \
        N_catch_balls += Pokemon_Encounter(catch_balls, pokemon);                                                \
        N_catch_bait += Pokemon_Encounter(catch_bait, pokemon);                                                  \
        N_catch_rock += Pokemon_Encounter(catch_rock, pokemon);                                                  \
        N_catch_bait_cheaty += Pokemon_Encounter(catch_bait_cheaty, pokemon);                                    \
        N_catch_rock_cheaty += Pokemon_Encounter(catch_rock_cheaty, pokemon);                                    \
    }                                                                                                            \
    OutputResult(#pokemon, N_catch_balls, N_catch_bait, N_catch_rock, N_catch_bait_cheaty, N_catch_rock_cheaty); \
    N_catch_balls = 0;                                                                                           \
    N_catch_bait = 0;                                                                                            \
    N_catch_rock = 0;                                                                                            \
    N_catch_bait_cheaty = 0;                                                                                     \
    N_catch_rock_cheaty = 0;

//Shortcuts
typedef unsigned int uint;

//Define type of catch strategy
enum catch_strat
{
    catch_balls,
    catch_bait,
    catch_rock,
    catch_bait_cheaty,
    catch_rock_cheaty
};

//Pokemon Class definition
class Pokemon
{
public:
    //Constructor must provide base flee rate and base catch rate
    Pokemon(uint flee_rate, uint catch_rate);
    bool Is_Fleeing() const;
    bool Try_Catch();
    bool Is_Eating() const { return (m_eating_turns > 0) ? true : false; }
    bool Is_Angry() const { return (m_angry_turns > 0) ? true : false; }
    bool Will_Be_Eating() const { return (m_eating_turns > 1) ? true : false; }
    bool Will_Be_Angry() const { return (m_angry_turns > 1) ? true : false; }
    void Throw_Bait();
    void Throw_Rock();

private:
    void UpdateStatus();
    uint m_flee_rate;
    uint m_catch_rate;
    uint m_angry_turns;
    uint m_eating_turns;
};

//Pokemon class functions
Pokemon::Pokemon(uint flee_rate, uint catch_rate) : m_flee_rate(flee_rate),
                                                    m_catch_rate(catch_rate),
                                                    m_angry_turns(0),
                                                    m_eating_turns(0)
{
}

bool Pokemon::Is_Fleeing() const
{
    //Throw random number 0-99
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<uint> distrib(0, 99);
    //We take the flee rate and factor it down, and converted into int
    //The factor is modified depending if the pokemon is angry or eating
    uint escape_factor;
    if (m_angry_turns > 0)
        escape_factor = std::clamp(uint(m_flee_rate * FACTOR_DOWN * 2.), (uint)1, (uint)1000);
    else if (m_eating_turns > 0)
        escape_factor = std::clamp(uint(m_flee_rate * FACTOR_DOWN / 4.), (uint)1, (uint)1000);
    else
        escape_factor = std::clamp(uint(m_flee_rate * FACTOR_DOWN * 1.), (uint)1, (uint)1000);
    //Factor 5 now
    escape_factor *= 5;
    //std::cout << escape_factor << std::endl;
    //Compare with random number, if it is lower, then it will flee
    return (distrib(gen) < escape_factor);
}

bool Pokemon::Try_Catch()
{
    //For a catch to be produced, we must pass 4 shaking checks
    //But first we must determine the catch factor, to be transformed into the probability to pass a shaking check
    uint catch_factor = m_catch_rate * FACTOR_DOWN;
    //This catch factor is affected by bait/rocks
    if (m_angry_turns > 0)
        catch_factor = std::clamp(catch_factor * 2, (uint)3, (uint)20);
    else if (m_eating_turns > 0)
        catch_factor = std::clamp(catch_factor / 2, (uint)3, (uint)20);
    //Now we factor up to retrieve the modified catching rate
    uint modified_catch_rate = catch_factor * FACTOR_UP;
    //This catch rate is used for the shaking formula, which for the safari zone reads: a = modified_catch_rate/2
    uint a = modified_catch_rate / 2;
    //With that we obtain the 1 to 65535 number dictating the shaking check probability
    uint b = 1048560 / uint(sqrt(uint(sqrt(uint(16711680 / a)))));
    //std::cout << b << std::endl;
    //Now we must pass 4 shaking checks, otherwise it will fail
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(0, 65535);
    for (uint i = 0; i < 4; i++)
    {
        if (distrib(gen) >= b)
        {
            UpdateStatus();
            return false;
        }
    }
    return true;
}

void Pokemon::Throw_Bait()
{
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(1, 5);
    m_angry_turns = 0; //No angry anymore
    m_eating_turns = std::clamp(m_eating_turns + distrib(gen), (uint)1, (uint)5);
}
void Pokemon::Throw_Rock()
{
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(1, 5);
    m_eating_turns = 0; //No eating anymore
    m_angry_turns = std::clamp(m_angry_turns + distrib(gen), (uint)1, (uint)5);
}

void Pokemon::UpdateStatus()
{
    m_angry_turns = (m_angry_turns == 0) ? 0 : std::clamp(m_angry_turns - 1, (uint)0, (uint)5);
    m_eating_turns = (m_eating_turns == 0) ? 0 : std::clamp(m_eating_turns - 1, (uint)0, (uint)5);
}

//Index of functions
int main();
bool Pokemon_Encounter(catch_strat strat, Pokemon pokemon);
void OutputResult(std::string const pokename, uint const N_catch_balls, uint const N_catch_bait, uint const N_catch_rock, uint const N_catch_bait_cheaty, uint const N_catch_rock_cheaty);
void PrepareOutput();

//MAIN
int main()
{
    //The goal is to do N_ENCOUNTERS with a pokemon (a Chansey in this case) and see how many times it is catched.
    //We try 3 strategies:
    //a) Only throw balls
    //b) Throw baits when it is not eating, balls otherwise
    //c) Throw baits following a regular pattern
    uint N_catch_balls = 0, N_catch_bait = 0, N_catch_rock = 0, N_catch_bait_cheaty = 0, N_catch_rock_cheaty = 0;
    PrepareOutput();
    POKELOOP(MAGIKARP);
    POKELOOP(NIDORANF);
    POKELOOP(NIDORANM);
    POKELOOP(PARAS);
    POKELOOP(VENONAT);
    POKELOOP(PSYDUCK);
    POKELOOP(POLIWAG);
    POKELOOP(SLOWPOKE);
    POKELOOP(DODUO);
    POKELOOP(GOLDEEN);
    POKELOOP(NIDORINO);
    POKELOOP(NIDORINA);
    POKELOOP(PARASECT);
    POKELOOP(VENOMOTH);
    POKELOOP(EXEGGCUTE);
    POKELOOP(RHYHORN);
    POKELOOP(SEAKING);
    POKELOOP(DRATINI);
    POKELOOP(CHANSEY);
    POKELOOP(KANGASKHAN);
    POKELOOP(SCYTHER);
    POKELOOP(PINSIR);
    POKELOOP(TAUROS);
    POKELOOP(DRAGONAIR);
    return 0;
}

bool Pokemon_Encounter(catch_strat strat, Pokemon pokemon)
{
    bool is_fleeing = false;
    //An encounter is made of turns, until either the pokemon escapes or it is caught.
    do
    {
        //Determine if pokemon will flee at the end of the turn
        is_fleeing = pokemon.Is_Fleeing();
        //Do action depending on strategy
        switch (strat)
        {
        case catch_balls: //Just throw a ball
            if (pokemon.Try_Catch())
                return true;
            break;
        case catch_bait: //Throw bait if pokemon isn't eating
            if (!pokemon.Is_Eating())
                pokemon.Throw_Bait();
            else
            {
                if (pokemon.Try_Catch())
                    return true;
            }
            break;
        case catch_rock: //Throw rock if pokemon isn't angry
            if (!pokemon.Is_Angry())
                pokemon.Throw_Rock();
            else
            {
                if (pokemon.Try_Catch())
                    return true;
            }
            break;
        case catch_bait_cheaty: //Throw bait when it will end after this turn (you cannoy know it without cheating!)
            if (!pokemon.Will_Be_Eating())
                pokemon.Throw_Bait();
            else
            {
                if (pokemon.Try_Catch())
                    return true;
            }
            break;
        case catch_rock_cheaty: //Throw rock when it will end after this turn (you cannoy know it without cheating!)
            if (!pokemon.Will_Be_Angry())
                pokemon.Throw_Rock();
            else
            {
                if (pokemon.Try_Catch())
                    return true;
            }
            break;
        }
    } while (!is_fleeing);
    return false;
}

void OutputResult(std::string const pokename, uint const N_catch_balls, uint const N_catch_bait, uint const N_catch_rock, uint const N_catch_bait_cheaty, uint const N_catch_rock_cheaty)
{
    //Output to pre-determined file: CatchRateTable.txt
    std::ofstream outf;

    outf.open(OUTFILE, std::ios_base::out | std::ios_base::app);
    outf << "| ";
    outf << std::left << std::setw(12) << std::setfill(' ') << pokename;
    outf << " | ";
    outf << std::left << std::setw(12) << std::setfill(' ') << N_catch_balls / double(N_ENCOUNTERS);
    outf << " | ";
    outf << std::left << std::setw(12) << std::setfill(' ') << N_catch_bait / double(N_ENCOUNTERS);
    outf << " | ";
    outf << std::left << std::setw(12) << std::setfill(' ') << N_catch_rock / double(N_ENCOUNTERS);
    outf << " | ";
    outf << std::left << std::setw(12) << std::setfill(' ') << N_catch_bait_cheaty / double(N_ENCOUNTERS);
    outf << " | ";
    outf << std::left << std::setw(12) << std::setfill(' ') << N_catch_rock_cheaty / double(N_ENCOUNTERS);
    outf << " |";
    outf << std::endl;
    outf.close();
}

void PrepareOutput()
{
    std::ofstream outf;

    outf.open(OUTFILE);
    outf << "| ";
    outf << std::left << std::setw(12) << std::setfill(' ') << "Pokemon";
    outf << " | ";
    outf << std::left << std::setw(12) << std::setfill(' ') << "Balls";
    outf << " | ";
    outf << std::left << std::setw(12) << std::setfill(' ') << "Bait";
    outf << " | ";
    outf << std::left << std::setw(12) << std::setfill(' ') << "Rock";
    outf << " | ";
    outf << std::left << std::setw(12) << std::setfill(' ') << "Cheaty bait";
    outf << " | ";
    outf << std::left << std::setw(12) << std::setfill(' ') << "Cheaty rock";
    outf << " |";
    outf << std::endl;

    outf << "|";
    outf << std::left << std::setw(14) << std::setfill('-') << "";
    outf << "|";
    outf << std::left << std::setw(14) << std::setfill('-') << "";
    outf << "|";
    outf << std::left << std::setw(14) << std::setfill('-') << "";
    outf << "|";
    outf << std::left << std::setw(14) << std::setfill('-') << "";
    outf << "|";
    outf << std::left << std::setw(14) << std::setfill('-') << "";
    outf << "|";
    outf << std::left << std::setw(14) << std::setfill('-') << "";
    outf << "|";
    outf << std::endl;
    outf.close();
}