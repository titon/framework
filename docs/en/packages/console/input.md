# Input #

All user input is handled by the `Input` class. This is where you define all input that is accepted in the command and/or application itself. The `Input` class also handles parsing of the input to be easily read later.

## Parameters ##

Parameters are any input given by the user into the command line application. The supported types of parameters are flags, options, arguments, and commands.

#### Description ####

The (optional) description of each parameter can also be set either in its constructor or via a `setter` method. This description is used when rendering the help screen.

```hack
$param->setDescription('This is a handy parameter!');
```

#### Aliases ####

Flags and options also support aliasing. Given the flag `--foo`, an alias assigned to it of `-f` will allow the user to retrieve the flag and assign values using either name.

```hack
$param = (new Flag('foo'))->alias('f');
```

#### Mode ####

The `mode` determines if user input is required for the paramter. If no input is given and the mode is set to be required, then an exception will be thrown.

The `mode` is set at construction or through a `setter` method. By default, all parameters are set to optional.

```hack
$param->setMode(AbstractInputDefinition::MODE_REQUIRED);
```

### Flags ###

A `Flag` is a boolean flag that is only checked by the parser for its existence. If the flag is present, it is given a value of 1 and a value of 0 is given if it is not.

**NOTE:** Although we say its a `boolean` parameter, that only means the parser does not attempt to give the parameter a value read from the user input. It only checks for the parameter's existence. The `value` assigned to flags are actually numeric to allow for stackable flags.

A flag is added to the `Input` object via the `addFlag` method.

```hack
$input->addFlag(new Flag('foo'));
```

Construction of a `Flag` is done by denoting a name for the flag, an optional description, a `mode` (if the flag is required or not), an a boolean value to determine if the flag is stackable.

```hack
$flag = new Flag('verbose', 'Set the verbosity level of the application', Flag::MODE_OPTIONAL, true);
```

A registered `Flag` object can be retrieved from the `Input` class by calling the `getFlag` method and passing in either the name or alias of the flag.

```hack
$myFlag = $input->getFlag('foo'); // Valid
$myFlag = $input->getFlag('f'); // Also valid
```

All registered flags can also be retrieved via the `getFlags` method.

#### Stackable Flags #####

If a flag is stackable, then the presence of multiple instances of the flag (consecutively) will increase the value of the flag.

```bash
-vvv // Given a value of 3
```

If the flag is not stackable, then it is either given a value of 1 (present) and 0 (if not present).

### Options ###

An `Option` is a value-based parameter that is assigned a value based on the input from the command line. The parser handles many different types of notations for reading in options:

```bash
// Long notation
--foo bar
--foo=bar
--foo="bar"

// Short notation
-f bar
-f=bar
-f="bar"
```

An option is added to the `Input` object via the `addOption` method.

```hack
$input->addOption(new Option('foo'));
```

**NOTE:** If a value containing spaces is to meant to be assigned to an option, it is required to be in quotes.

A registered `Option` object can be retrieved from the `Input` class by calling the `getOption` method and passing in either the name or alias of the option.

```hack
$myOption = $input->getOption('foo'); // Valid
$myOption = $input->getOption('f'); // Also valid
```

All registered options can also be retrieved via the `getOptions` method.

### Arguments ###

Arguments are parameters passed into the application that are not represented by any specific notation. For example, a 'hello world' application may accept a name argument and would be run as:

```hack
hhvm hello.hh "Alex Phillips"
```

An argument is added to the `Input` object via the `addArgument` method.

```hack
$input->addArgument(new Argument('foo'));
```

A registered `Argument` object can be retrieved from the `Input` class by calling the `getArgument` method and passing in the name of the argument.

```hack
$nameArgument = $input->getArgument('name');
```

All registered arguments can also be retrieved via the `getArguments` method.

## [Commands](commands.md) ##

Commands are added to the application through the `addCommand` method in the `Input` class. You can either preemptively parse a command out from the `Input` by calling `getActiveCommand`, or simply call `parse` which will parse out the command along with the rest of the registered parameters.

Once the input has been parsed, `getActiveCommand` will return whatever command has been parsed.

```hack
$input->addCommand(new MyCommand());
```

*See [Commands](commands.md) for more information.*

## User Input ##

The `Input` class also manages all user input via STDIN. This functionality is used in the [user input](user-input.md) classes for retrieving information from the user. If this functionality is needed outside of these classes, simply call `getUserInput` to prompt and return the entered value.

```hack
$usersValue = $input->getUserInput();
```
