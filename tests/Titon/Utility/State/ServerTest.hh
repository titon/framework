<?hh
namespace Titon\Utility\State;

use Titon\Test\TestCase;

class ServerTest extends TestCase {

    public function testClass(): void {
        $oldServer = $_SERVER;

        $_SERVER = [
            'HTTP_HOST' => 'localhost',
            'DOCUMENT_ROOT' => '/'
        ];

        Server::initialize($_SERVER);

        $this->assertEquals(Map {
            'HTTP_HOST' => 'localhost',
            'DOCUMENT_ROOT' => '/'
        }, Server::all());

        $this->assertEquals('localhost', Server::get('HTTP_HOST'));
        $this->assertTrue(Server::has('DOCUMENT_ROOT'));

        $this->assertEquals(null, Server::get('USER'));
        $this->assertFalse(Server::has('USER'));

        $_SERVER = $oldServer;
    }

}
