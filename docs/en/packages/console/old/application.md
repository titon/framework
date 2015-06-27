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

## Automatic Setup ##

The following style tags are also readily available for formatting output (see [`StyleDefinition`](style-definition.md) for more information):

* `<info>`: Green text, default background
* `<warning>`: Yellow text, default background
* `<error>`: Red text, default background
* `<exception>`: White text, red background
