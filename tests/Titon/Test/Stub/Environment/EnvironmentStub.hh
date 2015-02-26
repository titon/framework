<?hh
namespace Titon\Test\Stub\Environment;

use Titon\Environment\Environment;

class EnvironmentStub extends Environment {

    // Use host/IP for testing
    public function isMachine(string $name): bool {
        $host = '';

        if (array_key_exists('HTTP_HOST', $_SERVER)) {
            $host = $_SERVER['HTTP_HOST'];

        } else if (array_key_exists('SERVER_ADDR', $_SERVER)) {
            $host = $_SERVER['SERVER_ADDR'];
        }

        return (bool) preg_match('/^' . preg_quote($name, '/') . '/i', $host);
    }

}
