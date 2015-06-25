# Console #

The `Console` handles bootstrapping and running the `Application` object as well as providing all functionality that the [Kernel]() package provides.

## Setup ##

Setup of a new `Console` object (new CLI applications) is very simple. Since `Console` extends `Kernel`, we are required to pass in the `Application` as well as a new pipeline. Once the `Console` is ready to be ran, an `Input` and `Output` object is also required for handling all user input from the terminal and handling formatting output.

The `Pipeline` object handles management of middleware through queue data structures providing 

```hack
$app = new \Titon\Console\Application();
$kernel = new Console($app, new \Titon\Kernel\Middleware\Pipeline());
$kernel->run(new \Titon\Console\Input(), new \Titon\Console\Output());
$kernel->terminate();
