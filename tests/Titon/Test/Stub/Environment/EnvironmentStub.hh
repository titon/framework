<?hh // strict
namespace Titon\Test\Stub\Environment;

use Titon\Environment\Environment;

class EnvironmentStub extends Environment {

    // Use host/IP for testing
    public function isMachine(string $name): bool {
        $host = null;

        if (!empty($_SERVER['HTTP_HOST'])) {
            $host = $_SERVER['HTTP_HOST'];

        } else if (!empty($_SERVER['SERVER_ADDR'])) {
            $host = $_SERVER['SERVER_ADDR'];
        }

        return (bool) preg_match('/^' . preg_quote($name, '/') . '/i', $host);
    }

}
