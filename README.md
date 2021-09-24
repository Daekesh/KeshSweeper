# KeshSweeper
 Mine Sweeper recreated as a plugin for UE4 - play in the editor to kill time!

 ![alt text](https://github.com/Daekesh/KeshSweeper/blob/main/keshsweeper.png?raw=true)

 Implementation details:

- Gameplay 
  - Fully recreated the Mine Sweeper experience. I think.
  - The user can right click on a cell to "suspect" it of containing a mine.
  - The user can left click on a cell to reveal it.
  - Revealing a cell either explodes the mine underneath or unveils an empty space.
  - When revealing an empty space, it will either show a completely empty space or the number of
      adjacent mines (1-8.) If they are no adjacent mines, all adjacent cells are also revealed
      (in 8 directions) in a funky spiral pattern.
  - The reveal process is asynchronous. Each reveal will happen after a set time until there are
      no cells left to reveal (roughly every 0.1s.)
  - While the reveal process is ongoing, all user input on the minefield is suspended.
  - The game ends when either all non-mine spaces have been revealed or when a mine explodes.
  - When the game ends, all cells are revealed.
  
  - MVC approach
   - Model stores, and has authority over, the state of the current grid. Handles a maximum grid size
       of 255x255 and up to 65535 mines.
   - View displays the current grid and supplies options for new games. Makes sure to store the 
       authoratitive value of the sliders, rather than relying to the Slate objects, as the tab can
       be destroyed and recreated at any time. The view is persistent between tabs. When it is
       recreated, it displays exactly the same view as before.
   - Controller controls the logic of the game and starting new games.
   - The Plugin class itself serves as a container and pipeline for the MVC classes to communicate
       with each other.
   - Each MVC object has a post-instantiation Init and pre-destroy Destruct function so they can be
       inited after all 3 have been instantiated and cleaned up before all 3 are destroyed. Also
       so they don't take unneeded resources before the Editor loads.

- State machine driven
  - The state of the game is determined by an enum, EGameStatus (Not Started, In Progress, Won, Lost 
      and Error.) All interactions and flow are governed by this state.
  - Each cell in the grid is governed by a state, ECellStatus (Hidden, Suspected, Revealed and 
     Exploded,) and a mine flag.

- UI details
  - Done entirely in c++ using standard editor resources. No assets required.
  - Plenty of Slate code if you're looking for examples.
  - Button is added to the main toolbar to open the game tab.
  - Slider for the width and height of the grid. These change their maximum value depending on what
      will fit in the window. You can still change the size of the window after starting a game and
      hide part of the minefield, though. You can enter the height and width manually via text.
  - Slider for the difficulty. Values from 1 to 10. An algorithm sets the number of mines depending
      on the size of the grid determined by the width and height sliders. From grid size ^ 0.5 to 
      grid size ^ 0.9. You cannot manually enter the number of mines.
  - Status displays the current state of the game (Not started, In Progress, Won, Lost and Error.)
  - New game button discards the current game and starts a new one, based on the options selected.
  - The grid itself is represented by a Canvas object which handles mouse input. I figured it was
      more efficient to have a single "button" which works out which cell was clicked, rather than
      have, potentially, up to 65k buttons.
  - Each cell in the grid is represented by a state machine governed custom Slate object, derived
      from SOverlay.

  Have fun!
