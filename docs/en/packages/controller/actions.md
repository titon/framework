# Actions #

An action class is a sub-routine of the parent controller and is packaged as a stand-alone object instead of a controller method.
The class gives you the flexibility of re-using actions and specific logic across multiple controllers.

To use an action class, extend the `Titon\Controller\Action\AbstractAction` base class or implement the 
`Titon\Controller\Action` interface.

```hack
class ExampleAction extends Titon\Controller\Action\AbstractAction {
    // ...
}
```

[View the controller documentation for more information on controller method actions.](controllers.md)

## Mapping Requests ##

The `Action` interface defines 4 methods that map to HTTP methods, they are `get()`, `post()`, `put()`, and `delete()`. 
The type of action method executed will be determined by the HTTP method using in the request.

A typical CRUD (create, read, update, delete) action would be implemented like so.

```hack
class CrudAction extends Titon\Controller\Action\AbstractAction {
    public function post(): mixed {
        // POST: Create record and redirect to get()
    }
    
    public function get(int $id): mixed {
        // GET: Read record by ID
    }
    
    public function put(int $id): mixed {
        // PUT: Update a record by ID and redirect to get()
    }
    
    public function delete(int $id): mixed {
        // DELETE: Delete record by ID
    }
}
```

The remaining HTTP methods -- OPTIONS, PATCH, HEAD, TRACE, CONNECT -- will still be executed by the action, 
but are not required by the interface.

## Running An Action ##

To execute an action object, call the `runAction()` method within a controller method. 
Action classes will still require an action method within a controller.

```hack
public function actionName(): mixed {
    return $this->runAction(new CrudAction());
}
```

## Accessing The Controller ##

The parent controller in which the action was ran can be accessed at `getController()`. 
This is helpful in accessing methods, properties, or data that is required by the action, 
like the request and response objects.

```hack
public function get(): mixed {
    $controller = $this->getController();
    
    // ...
}
```