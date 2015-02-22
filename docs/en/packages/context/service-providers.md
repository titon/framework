# Service Providers #

Service providers provide a central location to bootstrap dependencies for an application. They allow the registering of
items into the Depository in groups as well as deferred loading so that items aren't registered until needed.

A service provider must extend `AbstractServiceProvider` and define the `register` method. An optional `provides` vector
can be defined. If the `provides` Vector is defined, all class names must be present in the list. This allows deferred
loading of the service provider and it will not be initialized until a class from the service provider is needed.

The `register` method is where the retrieving and registering items in the Depository is handled.

```hack
class SomeServiceProvider extends Titon\Context\ServiceProvider\AbstractServiceProvider {

    protected ClassList $provides = Vector {
        'Titon\Context\Foo'
    };

    public function register() {
        $this->depository->register('foo', 'Titon\Context\Foo');
    }
}
```
