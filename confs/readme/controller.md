# Controller #

Provides a `Controller` that acts as the median between the request and response within the dispatch cycle.
It splits up its responsibility into multiple actions, where each action deals with specific business logic.
The logical data is retrieved from a Model (database or logic entity) or from the request (POST, GET).

```hack
class FooController extends Titon\Controller\AbstractController {
    public function index(): mixed {
        // ...
    }
}

$foo = new FooController();
$foo->dispatchTo('index', $args);
```

An `Action` is a sub-routine of the parent controller and is packaged as a stand-alone object instead of a method.
An action object gives you the flexibility of re-using actions and specific logic across multiple controllers.
Each method within in action maps to an HTTP method.

```hack
class FormAction extends Titon\Controller\Action\AbstractAction {
    public function get(): mixed {
        // Load form
    }

    public function post(): mixed {
        // Process form
    }
}

$foo->runAction(new FormAction());
```