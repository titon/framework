<?hh
namespace Titon\Environment;

use Titon\Test\TestCase;

class HostTest extends TestCase {

    public function testGet() {
        $host = new Host(Server::DEV, ['dev', '123.0.0.0']);
        $host->setBootstrap(TEMP_DIR . '/dev.php');

        $this->assertEquals(Vector {'dev', '123.0.0.0'}, $host->getHostnames());
        $this->assertEquals(Server::DEV, $host->getType());
        $this->assertEquals(TEMP_DIR . '/dev.php', $host->getBootstrap());
    }

    public function testGetSetKey() {
        $host = new Host(Server::DEV, Vector {'dev', '123.0.0.0'});
        $this->assertEquals('', $host->getKey());

        $host->setKey('prod');
        $this->assertEquals('prod', $host->getKey());
    }

    public function testIsDevelopment() {
        $host = new Host(Server::DEV, '127.0.0.1');

        $this->assertTrue($host->isDevelopment());
        $this->assertFalse($host->isStaging());
        $this->assertFalse($host->isProduction());
    }

    public function testIsStaging() {
        $host = new Host(Server::STAGING, '127.0.0.1');

        $this->assertFalse($host->isDevelopment());
        $this->assertTrue($host->isStaging());
        $this->assertFalse($host->isProduction());
    }

    public function testIsProduction() {
        $host = new Host(Server::PROD, '127.0.0.1');

        $this->assertFalse($host->isDevelopment());
        $this->assertFalse($host->isStaging());
        $this->assertTrue($host->isProduction());
    }

}