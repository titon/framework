<?hh // strict
namespace Titon\Test\Stub\Context;

use Titon\Context\ServiceProvider\AbstractServiceProvider;

class BarServiceProviderStub extends AbstractServiceProvider {

    public function register(): void {
        // UNSAFE
        if ($this->depository !== null) {
            $this->depository->singleton('bar', 'Titon\Test\Stub\Context\BarStub')
                ->with('Titon\Test\Stub\Context\FooStub');
        }
    }

}
