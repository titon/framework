<?hh
namespace Titon\Utility;

use Titon\Test\TestCase;

class StrTest extends TestCase {

    protected $string = 'Titon: A PHP Modular Framework';
    protected $lipsum = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi eget tellus nibh.';

    public function testCharAt(): void {
        $this->assertEquals('T', Str::charAt('Titon', 0));
        $this->assertEquals('i', Str::charAt('Titon', 1));
        $this->assertEquals('t', Str::charAt('Titon', 2));
        $this->assertEquals('o', Str::charAt('Titon', 3));
        $this->assertEquals('n', Str::charAt('Titon', 4));
        $this->assertEquals(null, Str::charAt('Titon', 5));
        $this->assertEquals(null, Str::charAt('Titon', -1));
    }

    public function testCompare(): void {
        $this->assertEquals('0', Str::compare('foo', 'foo'));

        $this->assertLessThan('0', Str::compare('foo', 'Foobar'));
        $this->assertGreaterThan('0', Str::compare('foobar', 'Foo'));

        $this->assertEquals('0', Str::compare('foo', 'Foobar', 3));
        $this->assertGreaterThan('0', Str::compare('foobar', 'Foo', 5));
    }

    public function testContains(): void {
        $this->assertTrue(Str::contains($this->string, 'Titon'));
        $this->assertFalse(Str::contains($this->string, 'Zend'));

        // case-insensitive
        $this->assertTrue(Str::contains($this->string, 'TITON', false));

        // offset
        $this->assertFalse(Str::contains($this->string, 'Titon', true, 5));
    }

    public function testEndsWith(): void {
        $this->assertTrue(Str::endsWith($this->string, 'work'));
        $this->assertFalse(Str::endsWith($this->string, 'works'));

        // case-insensitive
        $this->assertTrue(Str::endsWith($this->string, 'WORK', false));
        $this->assertFalse(Str::endsWith($this->string, 'WORKS', false));
    }

    public function testExtract(): void {
        $this->assertEquals('Titon: A PHP Modular Framework', Str::extract($this->string, 0));
        $this->assertEquals('Titon', Str::extract($this->string, 0, 5));
        $this->assertEquals('Framework', Str::extract($this->string, -9));
        $this->assertEquals('Frame', Str::extract($this->string, -9, 5));
        $this->assertEquals('Modular', Str::extract($this->string, 13, 7));
    }

    public function testGenerate(): void {
        $this->assertNotEquals('ABCDEFGHIJ', Str::generate(10));
        $this->assertTrue(strlen(Str::generate(10)) == 10);

        $this->assertNotEquals('ABCDEFGHIJ12345', Str::generate(15));
        $this->assertTrue(strlen(Str::generate(15)) == 15);

        $this->assertEquals('aaaaa', Str::generate(5, 'aaaaa'));
        $this->assertTrue(strlen(Str::generate(5)) == 5);
    }

    public function testHash(): void {
        $md51 = Str::hash($this->string, 'md5');
        $md52 = Str::hash($this->string, 'md5', 'md5.salt');

        $this->assertTrue(strlen($md51) === 32);
        $this->assertTrue(strlen($md52) === 32);
        $this->assertNotEquals($md51, $md52);

        $sha1 = Str::hash($this->string, 'sha1');
        $sha2 = Str::hash($this->string, 'sha1', 'sha1.salt');

        $this->assertTrue(strlen($sha1) === 40);
        $this->assertTrue(strlen($sha2) === 40);
        $this->assertNotEquals($sha1, $sha2);

        $sha1 = Str::hash($this->string, 'sha256');
        $sha2 = Str::hash($this->string, 'sha256', 'sha256.salt');

        $this->assertTrue(strlen($sha1) === 64);
        $this->assertTrue(strlen($sha2) === 64);
        $this->assertNotEquals($sha1, $sha2);
    }

    public function testIndexOf(): void {
        $this->assertEquals(0, Str::indexOf($this->string, 'T'));
        $this->assertEquals(2, Str::indexOf($this->string, 't'));
        $this->assertEquals(7, Str::indexOf($this->string, 'A'));
        $this->assertEquals(13, Str::indexOf($this->string, 'M'));
        $this->assertEquals(21, Str::indexOf($this->string, 'F'));

        // Not found
        $this->assertEquals(-1, Str::indexOf($this->string, 'x'));

        // Case-insensitive
        $this->assertEquals(0, Str::indexOf($this->string, 'T', false));
        $this->assertEquals(0, Str::indexOf($this->string, 't', false));

        // Offset
        $this->assertEquals(3, Str::indexOf($this->string, 'o'));
        $this->assertEquals(14, Str::indexOf($this->string, 'o', true, 5));
    }

    public function testInsert(): void {
        $this->assertEquals('Titon is the best PHP framework around!', Str::insert('{framework} is the best {lang} framework around!', Map {
            'framework' => 'Titon',
            'lang' => 'PHP'
        }));

        $this->assertEquals('Titon is the best PHP framework around!', Str::insert(':framework is the best :lang framework around!', Map {
            'framework' => 'Titon',
            'lang' => 'PHP'
        }, Map {
            'before' => ':',
            'after' => ''
        }));
    }

    public function testLastIndexOf(): void {
        $this->assertEquals(0, Str::lastIndexOf($this->string, 'T'));
        $this->assertEquals(2, Str::lastIndexOf($this->string, 't'));
        $this->assertEquals(7, Str::lastIndexOf($this->string, 'A'));
        $this->assertEquals(13, Str::lastIndexOf($this->string, 'M'));
        $this->assertEquals(21, Str::lastIndexOf($this->string, 'F'));

        // Not found
        $this->assertEquals(-1, Str::indexOf($this->string, 'x'));

        // Case-insensitive
        $this->assertEquals(2, Str::lastIndexOf($this->string, 'T', false));
        $this->assertEquals(24, Str::lastIndexOf($this->string, 'M', false));
        $this->assertEquals(24, Str::lastIndexOf($this->string, 'm', false));

        // Offset
        $this->assertEquals(27, Str::lastIndexOf($this->string, 'o'));
        $this->assertEquals(27, Str::lastIndexOf($this->string, 'o', true, 5));
    }

    public function testListing(): void {
        $this->assertEquals('red, blue &amp; green', Str::listing(Vector {'red', 'blue', 'green'}));
        $this->assertEquals('red &amp; green', Str::listing(Vector {'red', 'green'}));
        $this->assertEquals('blue', Str::listing(Vector {'blue'}));
        $this->assertEquals('green', Str::listing(Vector {'green'}));

        // custom
        $this->assertEquals('red, blue, and green', Str::listing(Vector {'red', 'blue', 'green'}, ', and '));
        $this->assertEquals('red - blue and green', Str::listing(Vector {'red', 'blue', 'green'}, ' and ', ' - '));
    }

    public function testObfuscate(): void {
        $this->assertEquals('&#84;&#105;&#116;&#111;&#110;&#32;&#70;&#114;&#97;&#109;&#101;&#119;&#111;&#114;&#107;', Str::obfuscate('Titon Framework'));
    }

    public function testShorten(): void {
        $this->assertEquals('Lorem &hellip; nibh.', Str::shorten($this->lipsum, 10));
        $this->assertEquals('Lorem ipsum &hellip; tellus nibh.', Str::shorten($this->lipsum, 25));
        $this->assertEquals('Lorem ipsum dolor sit &hellip; Morbi eget tellus nibh.', Str::shorten($this->lipsum, 50));
        $this->assertEquals($this->lipsum, Str::shorten($this->lipsum, 100));

        // custom
        $this->assertEquals('Lorem ... nibh.', Str::shorten($this->lipsum, 10, ' ... '));
    }

    public function testStartsWith(): void {
        $this->assertTrue(Str::startsWith($this->string, 'Titon'));
        $this->assertFalse(Str::startsWith($this->string, 'Titan'));

        // case-insensitive
        $this->assertTrue(Str::startsWith($this->string, 'TITON', false));
        $this->assertFalse(Str::startsWith($this->string, 'TITAN', false));
    }

    public function testTruncate(): void {
        $string = 'This has <a href="/" class="link">anchor tags</a> &amp; entities in it. It has &quot;quotes&quot; as well.';

        // Preserve HTML and word
        $this->assertEquals('This has <a href="/" class="link">anchor</a>&hellip;', Str::truncate($string, 15));
        $this->assertEquals('This has <a href="/" class="link">anchor tags</a> &amp;&hellip;', Str::truncate($string, 25));
        $this->assertEquals('This has <a href="/" class="link">anchor tags</a> &amp; entities in&hellip;', Str::truncate($string, 35));
        $this->assertEquals('This has <a href="/" class="link">anchor tags</a> &amp; entities in it. It&hellip;', Str::truncate($string, 45));
        $this->assertEquals('This has <a href="/" class="link">anchor tags</a> &amp; entities in it. It has &quot;quotes&quot; as well.', Str::truncate($string, 0));

        // Preserve HTML
        $this->assertEquals('This has <a href="/" class="link">anch</a>&hellip;', Str::truncate($string, 13, Map {'word' => false}));
        $this->assertEquals('This has <a href="/" class="link">anchor tags</a> &amp; en&hellip;', Str::truncate($string, 25, Map {'word' => false}));
        $this->assertEquals('This has <a href="/" class="link">anchor tags</a> &amp; entities in&hellip;', Str::truncate($string, 35, Map {'word' => false}));
        $this->assertEquals('This has <a href="/" class="link">anchor tags</a> &amp; entities in it. It has&hellip;', Str::truncate($string, 45, Map {'word' => false}));
        $this->assertEquals('This has <a href="/" class="link">anchor tags</a> &amp; entities in it. It has &quot;quo&hellip;', Str::truncate($string, 50, Map {'word' => false}));

        // Preserve none
        $this->assertEquals('This has anchor tags &amp; en&hellip;', Str::truncate($string, 25, Map {'word' => false, 'html' => false}));
        $this->assertEquals('This has anchor tags &amp; entities in&hellip;', Str::truncate($string, 35, Map {'word' => false, 'html' => false}));
        $this->assertEquals('This has anchor tags &amp; entities in it. It has&hellip;', Str::truncate($string, 45, Map {'word' => false, 'html' => false}));
        $this->assertEquals('This has anchor tags &amp; entities in it. It has &quot;quotes&quot; as well.', Str::truncate($string, 0, Map {'word' => false, 'html' => false}));

        // Preserve none with custom suffix
        $this->assertEquals('This has anchor tags &amp; en...', Str::truncate($string, 25, Map {'word' => false, 'html' => false, 'suffix' => '...'}));
        $this->assertEquals('This has anchor tags &amp; entities in...', Str::truncate($string, 35, Map {'word' => false, 'html' => false, 'suffix' => '...'}));
        $this->assertEquals('This has anchor tags &amp; entities in it. It has...', Str::truncate($string, 45, Map {'word' => false, 'html' => false, 'suffix' => '...'}));
        $this->assertEquals('This has anchor tags &amp; entities in it. It has &quot;quotes&quot; as well.', Str::truncate($string, 0, Map {'word' => false, 'html' => false, 'suffix' => '...'}));

        // Custom tags (BB code)
        $string = 'This has [url="/"]anchor tags[/url] &amp; entities in it. It has &quot;quotes&quot; as well.';

        $this->assertEquals('This has [url="/"]anchor[/url]&hellip;', Str::truncate($string, 15, Map {'open' => '[', 'close' => ']'}));
        $this->assertEquals('This has [url="/"]anchor tags[/url] &amp;&hellip;', Str::truncate($string, 25, Map {'open' => '[', 'close' => ']'}));
        $this->assertEquals('This has [url="/"]anchor tags[/url] &amp; entities in&hellip;', Str::truncate($string, 35, Map {'open' => '[', 'close' => ']'}));
        $this->assertEquals('This has [url="/"]anchor tags[/url] &amp; entities in it. It&hellip;', Str::truncate($string, 45, Map {'open' => '[', 'close' => ']'}));
        $this->assertEquals('This has [url="/"]anchor tags[/url] &amp; entities in it. It has &quot;quotes&quot; as well.', Str::truncate($string, 0, Map {'open' => '[', 'close' => ']'}));
    }

    public function testUuid(): void {
        $this->assertRegExp('/^[a-f0-9]{8}\-[a-f0-9]{4}\-4[a-f0-9]{3}\-[89AB]{1}[a-f0-9]{3}\-[a-f0-9]{12}$/i', Str::uuid());
        $this->assertRegExp('/^[a-f0-9]{8}\-[a-f0-9]{4}\-4[a-f0-9]{3}\-[89AB]{1}[a-f0-9]{3}\-[a-f0-9]{12}$/i', Str::uuid());
        $this->assertRegExp('/^[a-f0-9]{8}\-[a-f0-9]{4}\-4[a-f0-9]{3}\-[89AB]{1}[a-f0-9]{3}\-[a-f0-9]{12}$/i', Str::uuid());
    }

}
