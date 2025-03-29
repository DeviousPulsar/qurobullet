# qurobullet

A powerful 2D projectile system module for Godot!

[Check out the release demo video here!](https://www.youtube.com/watch?v=Pye_jmW4an0)

**qurobullet** provides a means of easily creating and processing thousands of configurable 2D projectiles in Godot. It handles the heavy-lifting of spawning, movement, and collision detection for large groups of bullets, and gives the user tools for designing bullet-hell-worthy patterns!

It consists of four main components: the `BulletServer` and `BulletSpawner` nodes, and the `BulletPath` and `BulletTexture` resources.

- A `BulletServer` creates a configurable pool of bullet objects (not nodes), controls their movement, and reports their collisions. The bullet pool does not grow, and recycles the oldest bullets if overloaded.
- A `BulletSpawner` calculates various arrangements to spawn bullets in, and "spawns" bullets by sending a signal to a `BulletServer` containing the positions, directions, `BulletPath`, `BulletTexture`, and custom data desired. It features a preview drawing system which shows the positions and directions their bullets will travel, which simplifies the process of creating new patterns via `AnimationPlayer` .
- A `BulletPath` provides the information about the bullet's motion through the use of it's `get_step` function. This function calculates the motion of the bullet over that a time step. There are 3 provided implementations of `BulletPath`: `SimpleBulletPath`, `BezierBulletPath`, and `ForceBulletPath`.
- A `BulletTexture` provieds the information about the bullet's texture and collision.

More specific information about these components and their properties can be found in the built-in documentation.

## Installation

Because **qurobullet** is a module, it must be added to the modules folder of Godot's source and compiled into a custom build. If you are unfamiliar with the process of compiling software from source, [Godot's documentation can point you in the right direction for any platform](https://docs.godotengine.org/en/stable/development/compiling/index.html).

This module does not have dependencies beyond those required to build Godot.

Due to configuration issues, this module must be compiled with gcc/mingw - I will fix this at a later date.

The installation method can vary a bit, depending on your OS and whether you're cloning Godot's git repo or downloading a zipped version (and whether you want to keep the module up to date with this repository), but so long as all of the source files from this repository end up in `<your_godot_source_folder>/modules/qurobullet`, you're all set to compile the engine as normal, and **qurobullet** will be included!

If you wish to clone this module into your own Godot git repository, I would recommend adding it as a submodule. This will allow you to keep them up to date independently without interfering with each other. To do so, open a terminal and enter the following:

    cd <your_godot_source_folder>/modules
    git submodule add https://github.com/quinnvoker/qurobullet

This will clone this repo into your modules folder as a submodule, and you're ready to compile!

## How to use

Once you've added the module to your Godot build, using **qurobullet** in your project is straightforward. Add a `BulletServer` and `BulletSpawner` to your scene, define a `BulletType` for the spawner to use, and you're done!

No manual signal connections are required for these components, because they automatically connect themselves to the included `BulletServerRelay` singleton object on `ready`. Because of this, one server object can receive signals from all spawners in any given scene (including those within instanced scenes), without needing configuration! This can be disabled, and connections can also be made manually. This is handy if your project specifically requires multiple `BulletServer` nodes for different uses (for example, a separate server for the player's bullets so they can be recycled with high efficiency, or just to have separate visual layers of bullets).

Because bullets are objects rather than area or body nodes, their collision shapes are not monitorable, and they will not trigger emission of any `area_entered` or `body_entered` signals from `CollisionObject2D`s in your scene. Instead, when a bullet collides with something, the `BulletServer` running it will emit a `collision_detected` signal, with the bullet and an array containing the `CollisionObject2D`s it has contacted as arguments. You are free to process collisions in your game however you like using this signal. You could connect it to a function which reads the bullet's type data and uses it to apply the appropriate amount of damage to the colliders it hit that frame, or perhaps just read the bullet type's `custom_data` and play some `AudioStream` you have stored there, for example.

## Modularity

Because all communication between components is done via signals, **qurobullet** is naturally modular. You can write your own custom spawners and connect them to the `BulletServerRelay` or a `BulletServer` seamlessly, so long as you provide the required data!

`BulletServer` has two public methods for spawning bullets, `spawn_bullet`, and `spawn_volley`.

`spawn_bullet` spawns a single bullet, and expects the following arguments:

- The `BulletPath` and `BulletTexture` to use
- The starting position of the bullet in world space, as a `Vector2`
- The direction in which the bullet will travel, as a `Vector2` (must be normalised)
- A optional `Dictionary` of custom data to be added to the bullet

`spawn_volley` spawns any number of bullets at once, and expects the following arguments:

- The `BulletPath` and `BulletTexture` to use
- The origin of the spawner in world space, as a `Vector2`
- An `Array` containing spawn info for each shot. This spawn info is stored as a `Dictionary` containing two entries, _"position"_ (bullet spawn position relative to origin, as a `Vector2`), and _"direction"_ (bullet travel direction, also a `Vector2`, and must be normalised)
- A optional `Dictionary` of custom data to be added to all the bullets

Knowing this, you can spawn bullets any way you wish via code! You could even code your own `BulletServer` implementation, if all you needed was a configurable spawner and bullet data definition...

In addition you may create custom implementations of `BulletPath`, thus allowing for fully customizable bullet trajectories and behaviors. See the documentation for more details on the `BulletPath` type

## Known Issues

- Combining the use of `arc_offset` and `MANUAL` pattern mode in `BulletSpawner` is not recommended and results in undesired behaviour, because `arc_offset`'s shot wrapping (applied when a shot would be offset beyond the arc's range) modifies shot indices.
- Bullets set to collide with Area2Ds will ignore their `monitorable` status and always see them.
- `BulletSpawner` does its best to prevent "stacking" bullets by combining them into one if both their directions and spawn positions are approximately equal when spawned. However, this is not a guarantee, and spawners can still stack bullets when shots are scattered individually.
