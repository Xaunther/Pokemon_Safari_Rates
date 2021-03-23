# Pokemon_Safari_Rates

Script to compute the probability to catch a pokemon once it's been encountered in the safari zone.
It assumes several things:

* Safari balls are infinite.
* A bait (rock) will make the pokemon eat (angry) for 1-5 BALL THROWS (stackable up to 5).
* Base catch and flee rates obtained from the [bulbapedia](https://m.bulbapedia.bulbagarden.net/wiki/Kanto_Safari_Zone).
* Using [catch rate](https://bulbapedia.bulbagarden.net/wiki/Catch_rate#Capture_method_.28Generation_III-IV.29) for Gen III, which simplifies for the safari zone because current HP = max HP, status bonus is always 1 and ball bonus is always 1.5.
* 4 shake checks are required for a successful catch.
* Chansey bait "bug" is reproduced, meaning its catch rate increases while eating.
* Whether the pokemon is going to flee at the end of current turn is determined before player action.
* 5 strategies are checked:
  * Balls: Throw safari balls at every turn.
  * Bait: Throw bait when not eating, attempt catch while eating.
  * Rock: Throw rock when not angry, attempty catch while angry
  * Cheaty bait: Perfectly throw bait right before pokemon stops eating or is not eating, attempt catches otherwise.
  * Cheaty rock: Perfectly throw rocks right before pokemon stops being angry or is not angry, attempty catches otherwise.

## Usage

Simply compiling main.cpp, std++17 required. There's also the possibility to use cmake.
The scripts produces a txt file with the probabilities for each strategy and pokemon, by default: [CatchRateTable.txt](CatchRateTable.txt).
