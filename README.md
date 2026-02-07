<p align="center">
  <img width="400" height="215" alt="logo" src="https://github.com/user-attachments/assets/87001539-7f56-471a-88f3-2476df0fcec8" />
</p>

<h2 align="center">Avalanche : A C++ Falling Sand Engine</h2>

<div align="center">

**[Introduction](#introduction)** • **[Showcase](#showcase)** • **[Features](#features)** • **[Integration](#integration)** • **[API Design](#api-design)** • **[Basic Example](#basic-example)** • **[Demos](#demos)** • **[References](#references)**

</div>

<h2>Introduction</h2>

A production-ready falling sand physics engine designed as a single-header C++ library. Able to simulate thousands of cells on the CPU that developers can integrate into their projects with minimal friction. In Avalanche, all cell behavior is user defined via a clean API, allowing for easy extension and in-depth customization.

<h2>Showcase</h2>

https://github.com/user-attachments/assets/8ba8f900-1486-4bef-95c3-fd02e29d49b6

<h5 align="center"><i>Here is a video showcasing some of the behaviors that can be simulated using the API.</i></h5>

<h2>Features</h2>

* **Multithreaded** - Automatically distributes cell processing across threads for increased performance.
* **Deterministic** - Same starting conditions always produce the same results.
* **Drop-in integration** - Designed as a single-header library that can integrated into projects with minimal friction.
* **User defined cell logic** - Create your own materials and behaviors through simple callbacks without modifying engine code.
* **Complex cell behaviors** - Store custom data per cell to handle more complex states like fire spread, aging, temperature, and more.
* **Core utilities** - Built-in functions for plotting cells, circles, lines, and rectangles and checking cell states and more.

<h2>Integration</h2>

Avalanche is designed as a single-header library for easy integration into your project. Simply download or copy the `avalanche.hpp` header file and include it in your source code:
```cpp
#include "avalanche.hpp"
```

In **exactly one** of your `.cpp` files, define the implementation before including the header:
```cpp
#define AVALANCHE_IMPLEMENTATION
#include "avalanche.hpp"
```

This tells the compiler to include the actual function implementations in that translation unit, preventing multiple definition errors during linking while still allowing you to include the header normally in other files.

That's it! No additional dependencies or build configurations required. The library is self-contained and ready to use immediately.

For a complete working example, check out the [demos](https://github.com/PiterGroot/avalanche/tree/main/demos) which show how to set up a world, register cells, and integrate with different rendering backends.

<h2>API design</h2>
One of the project goals is to provide a clean and flexible API that allows users to define custom cell types and behaviors easily. Users can define their own cell types and behaviors using function pointers. This is all possible with the `CellUpdateContext` struct.

<br>This struct is a context object passed to cell update functions, providing them with all the necessary information and utilities to perform their behavior. This allows users to create new cell types without modifying the engine code itself, making it much more flexible and extensible.

<h2>Basic example</h2>
Here is an example of how a user-defined cell update function can look like:<br><br>

```cpp
void UpdateSandCell(avl::CellUpdateContext& ctx)
{
	const int targetY = ctx.y + 1;
	if (!ctx.is_out_of_bounds(ctx.x, targetY)) // Is the desired position out of bounds in the current sector?
	{
		uint8_t downCellID = 0;
		if (ctx.is_empty(ctx.x, targetY, downCellID)) // Is the desired position empty? Cache the checked cellID for later.
		{
			ctx.move_cell(ctx.x, targetY); // Successfully move to the desired position.
		}
		else if (ctx.cellID != WATER_CELL_ID && downCellID == WATER_CELL_ID) // Check if there is water underneath this cell.
		{
			if (avl::utils::get_chance(.5f)) // Chance to swap donwards to simulate sand sinking in water.
				ctx.swap_cell(ctx.x, targetY);
			else
				ctx.notify_chunk();
		}
	}
}
```

In this example, the UpdateSandCell function defines the behavior of a sand cell using the given CellUpdateContext struct. It checks the cell directly below it, and if that cell is empty, it moves down. If the cell below is water, it swaps places with the water cell half of the time, simulating sand sinking through water. The function uses methods provided by the CellUpdateContext to interact with the simulation, such as checking for out-of-bounds conditions, determining if a cell is empty, moving cells, and swapping cells.

To register this cell just call the global function <code>register_cell</code> in the `avl` namespace. In this example, I register a sand cell with ID 4, a bright orange color, and associate it with the UpdateSandCell function shown above.

```cpp
avl::utils::register_cell
(
    4,                                                  // Cell ID.
    avl::utils::pack_RGBA(1, 0.984f, 1.0f, 1.0f),       // Packed cell color (RGBA). 
    *world,                                             // Pointer to the world. 
    UpdateSandCell,                                     // Cell update function.
);
```

Additionally, cells can be registered with default userdata attached. Every cell has been allocated one `uint32_t` of data, which the user can use and interpret however they want. These could be simple bitflags, counters ect. 

```cpp
avl::utils::register_cell
(
    8,                                                  // Cell ID.
    avl::utils::pack_RGBA(1, 0.369f, 0.0f, 1.0f),       // Packed cell color. 
    *world,                                             // Pointer to the world. 
    OnUpdateFire,                                       // Cell update function.
    avl::CellUserData(HOT | SHORT_LIFE_TIME, 0).pack()  // Initial user data of a fire cell (flags + value).
);
```

Take a look at the [demos](https://github.com/PiterGroot/avalanche/tree/main/demos) for a simple complete implementation. See below a brief summery of every current demo.

<h2>Demos</h2>

All demos share code for all the example cells. If you are courious about those implementation, you can check those out inside the project `DemoCore` [here](https://github.com/PiterGroot/avalanche/tree/main/demos/DemoCore/DemoCore/include)

CMake is the easiest way to build the demo projects, to build, clone or download the repo and go to the root folder then:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

<h2></h2>

<h4>RaylibDemo</h4>

A demo project showcasing an implementation using [Raylib](https://www.raylib.com/) for the backend rendering using simple textures and usage of the various handy callbacks.

https://github.com/user-attachments/assets/2a3c4335-3fd1-4aea-9b7d-88a3d8c8b6b1

<h4>ASCIIDemo</h4>

A demo project showcasing the backend-agnostic design of avalanche. This project runs using the windows API in a simple console using ASCII characters for the different cell types.

https://github.com/user-attachments/assets/4e6d0c77-0614-44bc-ae2e-28efa7c5a1f8

<h4>Benchmarker</h4>

A demo project that runs avalanche headless to simulate different falling-sand configurations to gather some performance insights on increasingly complex scenes.

https://github.com/user-attachments/assets/3bf80b1a-c449-4817-9a19-6c573401da1b

<h2>References</h2>
This library was inspired by and built upon the following resources and projects: <br><br>

<ul>
                    <li><strong>Noita</strong> by Nolla Games - A pixel-based action roguelike that inspired the falling sand engine concept. Visit <a href="https://noitagame.com/">noitagame.com</a> and read about <a href="https://nollagames.com/fallingeverything/">the falling everything engine</a>.</li>
                    <li><strong>"Exploring the Tech and Design of Noita"</strong> - GDC talk by Petri Purho discussing multithreading approaches in falling sand simulations. Watch on <a href="https://www.youtube.com/watch?v=prXuyMCgbTc">YouTube</a>.</li>
                    <li><strong>"Recreating Noita's Sand Simulation in C and OpenGL | Game Engineering"</strong> - Educational resource by John Jackson explaining sand simulation fundamentals. Watch on <a href="https://www.youtube.com/watch?v=VLZjd_Y1gJ8">YouTube</a>.</li>
                    <li><strong>"How To Code a Falling Sand Simulation (like Noita) with Cellular Automata"</strong> - Tutorial by MARF covering row-based multithreading patterns. Watch on <a href="https://www.youtube.com/watch?v=5Ka3tbbT-9E">YouTube</a>.</li>
                    <li><strong>Winter Dev's Falling Sand Project</strong> - An inspiring falling sand implementation that influenced the design decisions in this project. Visit <a href="https://winter.dev/articles/falling-sand">winter.dev/articles/falling-sand</a>.</li>
                    <li><strong>Music in showcase video</strong> - Blue Danube - Strauss | <a href="https://creativecommons.org/licenses/by/3.0/">Creative Commons ► Attribution 3.0 Unported ► CC BY 3.0</a>.</li>
</ul>

These resources provided invaluable insights into performance optimization, API design, and simulation techniques that shaped the development of this project.
