
import curses
from curses import textpad

def main(stdscr):
    # Clear screen
    stdscr.clear()

    # Set up colors
    curses.init_pair(1, curses.COLOR_RED, curses.COLOR_BLACK)
    RED = curses.color_pair(1)

    # Define game states and endings
    game_states = {
        "start": {
            "message": "You are in a dark forest. A dragon threatens your kingdom. What will you do?",
            "choices": [
                ("Fight the dragon!", "fight"),
                ("Try to find another way...", "find_way")
            ]
        },
        "fight": {
            "message": "The dragon breathes fire at you! You have 50 HP left.",
            "choices": [
                ("Dodge and attack!", "win_fight"),
                ("Run away!", "lose_fight")
            ]
        },
        "find_way": {
            "message": "You find a hidden path through the mountains. What do you do next?",
            "choices": [
                ("Go back to fight.", "back_to_fight"),
                ("Continue to safety...", "escape")
            ]
        }
    }

    current_state = "start"
    player_hp = 100

    while True:
        state = game_states[current_state]

        # Display current message
        stdscr.addstr(0, 0, state["message"], RED)

        # Display choices
        for i, (choice_text, choice_value) in enumerate(state["choices"]):
            stdscr.addstr(i + 2, 0, f"{i + 1}. {choice_text}", RED)

        # Get user input
        key = stdscr.getch()

        if key == curses.KEY_UP:
            continue

        try:
            choice_index = int(key) - 1
        except ValueError:
            continue

        if 0 <= choice_index < len(state["choices"]):
            next_state = state["choices"][choice_index][1]

            if next_state == "win_fight":
                stdscr.addstr(5, 0, "You defeat the dragon and save your kingdom! Congratulations!", RED)
                break
            elif next_state == "lose_fight":
                stdscr.addstr(5, 0, "The dragon's fire breath reduces you to ashes. Game Over.", RED)
                break
            elif next_state == "back_to_fight":
                current_state = "fight"
            elif next_state == "escape":
                stdscr.addstr(5, 0, "You escape the forest and live to tell your tale!", RED)
                break

        # Wait for key press before continuing
        stdscr.getch()

if __name__ == "__main__":
    curses.wrapper(main)