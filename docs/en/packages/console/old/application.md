# Application #

See [`Console`](console.md) for more information on scaffolding and building applications.

The `Console` object functions as the `application` of the console package. Used in conjuction with the `Console`, it provides all necessary scaffolding and setup for building and running CLI applications.

## Construction ##

Creating an application is as simple as constructing a new `Application` object and optionally passing in `Input` and `Output` objects. If no `Input` object is passed in, a new one is constructed using the global arguments provided by the PHP/Hack language.

When used with the `Console`, the `Input` and `Output` objects will be replaced by those passed into the console's `run` method.

```hack
$app = new Console();
```

## Commands ##

Once a new application has been created, add commands by calling `addCommand` and passing in a new instance of the command itself. The `Application` object will take care of setting up and configuring the `Command` as necessary.

````hack
$app->addCommand(new MyCommand());
```

## Running the Application ##

Once all necessary commands have been added, calling `run` on the `Application` object will execute the application. Again, when used with the `Console` package, this is all handled when the `Console` itself is ran.

```hack
$app->run();
```

When executing the application from the command line, the `Application` object will handle the following situations:

* If no command is provided or the `help` flag is present, the help screen of the application will be presented listing all global flags, global options, and available commands.
* If a command is present and the `help` flag is present, the help screen of the given command will be presented listing all available options, flags, and arguments as well as the usage of the command and its description.
* If the command is present with valid parameters, the application will execute the `run` command of the `Command`.

## Automatic Setup ##

The `Console` class will automatically bootstrap and set up global parameters and output styles ready for use. The following parameters are readily available:

* --help (-h): The `help` flag to automatically render the help screen of the application or given command.
* --quiet (-q): Set the verbosity of the application to 0, suppressing all output.
* --verbose (-v) [stackable]: Set the verbosity level of the application. This is stackable, so each instance of `v` will increase the verbosity level by 1.
* --ansi: Force ANSI output
* --no-ansi: Suppress all ANSI output

The following style tags are also readily available for formatting output (see [`StyleDefinition`](style-definition.md) for more information):

* `<info>`: Green text, default background
* `<warning>`: Yellow text, default background
* `<error>`: Red text, default background
* `<exception>`: White text, red background
