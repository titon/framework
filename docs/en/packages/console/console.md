# Console #

The `Console` object functions as the `application` of the console package.

## Construction ##

Creating an application is as simple as constructing a new `Console` object and optionally passing in `Input` and `Output` objects. If no `Input` object is passed in, a new one is constructed using the global arguments provided by the PHP/Hack language.

```hack
$app = new Console();
```

## Commands ##

Once a new application has been created, add commands by calling `addCommand` and passing in a new instance of the command itself. The `Console` object will take care of setting up and configuring the `Command` as necessary.

````hack
$app->addCommand(new MyCommand());
```

## Running the Application ##

Once all necessary commands have been added, calling `run` on the `Console` object will execute the application.

```hack
$app->run();
```

When executing the application from the command line, the `Console` object will handle the following situations:

* If no command is provided or the `help` flag is present, the help screen of the application will be presented listing all global flags, global options, and available commands.
* If a command is present and the `help` flag is present, the help screen of the given command will be presented listing all available options, flags, and arguments as well as the usage of the command and its description.
* If the command is present with valid parameters, the `Console` application will execute the `run` command of the `Command`.
