<?hh
namespace Titon\Test\Stub\Context;

use Titon\Context\ClassList;
use Titon\Context\ServiceProvider\AbstractServiceProvider;

class FooServiceProviderStub extends AbstractServiceProvider {

    protected ClassList $provides = Vector {
        'Titon\Test\Stub\Context\FooStub'
    };

    public function register() {
        $this->depository->register('foo', 'Titon\Test\Stub\Context\FooStub');
    }

}
