<?hh // strict
namespace Titon\Test\Stub\Environment;

use Titon\Environment\Bootstrapper;
use Titon\Environment\Host;

class BootstrapperStub implements Bootstrapper {

    public static string $loaded = '';

    public function bootstrap(Host $host): void {
        static::$loaded = $host->getKey();
    }

}
