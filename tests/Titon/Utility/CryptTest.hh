<?hh
namespace Titon\Utility;

use Titon\Test\TestCase;

class CryptTest extends TestCase {

    protected $string = 'This string will be used to encrypt and decrypt. It contains numbers: 1 2 3 4 5 6 7 8 9 0. It also contains punctuation: ! @ # $ % ^ & * ( ) : [ < ?. You get the picture';

    /*public function testEncryptDecrypt(): void {
        $e = Crypt::encrypt($this->string, 'cast.key', 'AES-128-CBC');
        $d = Crypt::decrypt($e, 'cast.key', 'AES-128-CBC');

        $this->assertEquals($this->string, $d);

        $e = Crypt::encrypt($this->string, 'xtea.key', 'AES-128-CBC');
        $d = Crypt::decrypt($e, 'xtea.key', 'AES-128-CBC');

        $this->assertEquals($this->string, $d);
    }*/

}
