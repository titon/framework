<?hh
namespace Titon\Utility\State;

use Titon\Test\TestCase;

class EnvTest extends TestCase {

    public function testClass() {
        $oldEnv = $_ENV;

        $_ENV = [
            'HOME' => '/home/vagrant',
            'PATH' => '/usr/local/bin'
        ];

        Env::initialize($_ENV);

        $this->assertEquals(Map {
            'HOME' => '/home/vagrant',
            'PATH' => '/usr/local/bin'
        }, Env::all());

        $this->assertEquals('/home/vagrant', Env::get('HOME'));
        $this->assertTrue(Env::has('PATH'));

        $this->assertEquals(null, Env::get('USER'));
        $this->assertFalse(Env::has('USER'));

        $_ENV = $oldEnv;
    }

}