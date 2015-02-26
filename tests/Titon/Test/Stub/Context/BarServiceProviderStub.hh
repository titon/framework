<?hh
namespace Titon\Test\Stub\Context;

use Titon\Context\ServiceProvider\AbstractServiceProvider;

class BarServiceProviderStub extends AbstractServiceProvider {

    public function register() {
        $this->depository->singleton('bar', 'Titon\Test\Stub\Context\BarStub')
            ->with('Titon\Test\Stub\Context\FooStub');
    }

}
