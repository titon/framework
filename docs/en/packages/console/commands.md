# Commands #

A `Command` is a user-created class that handles a specific process with executed by the user from a `Console` application.

## Setup ##

A `Command` object is required to have at least 2 methods: `configure` and `run`.

The `configure` method is where the user will specify the name of the command, an optional description, and register any command-specific parameters to be used at runtime.

The `run` method is executed by the `Console` after all necessary input is processed.

```hack
class HelloWorldCommand {

    public function configure() {
        $this->setName("hello")->setDescription("A simple 'hello world' command!");
        $this->addArgument(new Argument("name"));
    }

    public function run() {
        if (!is_null($name = $this->getArgument('name'))) {
            $this->out("Hello, $name!");
        } else {
            $this->out("Hello, world!");
        }
    }
}
```

## Accessing Parameters ##

As shown above, the `getArgument` method inside of the command returns a value as opposed to the `Input` class which returns the `Argument` object. The methods `getFlag`, `getOption`, and `getArgument`, when used inside of the `Command`, will return the value (or default) of the registered parameter. If there is a necessity to access the actual objects, these can be retrieved by calling the same commands on the class's `Input` variable.

```hack
$nameArgument = $this->input->getArgument('name');
```
