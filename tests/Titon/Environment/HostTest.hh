<?hh
namespace Titon\Environment;

use Titon\Test\TestCase;

class HostTest extends TestCase {

    /**
     * @expectedException \Titon\Environment\Exception\InvalidEnvironmentException
     */
    public function testErrorsOnInvalidType() {
        new Host('localhost', 'foobar');
    }

    public function testGet() {
        $host = new Host(['dev', '123.0.0.0']);
        $host->setBootstrap(TEMP_DIR . '/dev.php');

        $this->assertEquals(Vector {'dev', '123.0.0.0'}, $host->getHosts());
        $this->assertEquals(Environment::DEVELOPMENT, $host->getType());
        $this->assertEquals(TEMP_DIR . '/dev.php', $host->getBootstrap());
    }

    public function testGetSetKey() {
        $host = new Host(Vector {'dev', '123.0.0.0'});
        $this->assertEquals('', $host->getKey());

        $host->setKey('prod');
        $this->assertEquals('prod', $host->getKey());
    }

    public function testIsDevelopment() {
        $host = new Host('127.0.0.1');

        $this->assertTrue($host->isDevelopment());
        $this->assertFalse($host->isStaging());
        $this->assertFalse($host->isProduction());
    }

    public function testIsStaging() {
        $host = new Host('127.0.0.1', Environment::STAGING);

        $this->assertFalse($host->isDevelopment());
        $this->assertTrue($host->isStaging());
        $this->assertFalse($host->isProduction());
    }

    public function testIsProduction() {
        $host = new Host('127.0.0.1', Environment::PRODUCTION);

        $this->assertFalse($host->isDevelopment());
        $this->assertFalse($host->isStaging());
        $this->assertTrue($host->isProduction());
    }

}