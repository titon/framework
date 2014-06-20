<?php
namespace Titon\Utility;

use Titon\Test\TestCase;

class StringTest extends TestCase {

    protected $string = 'Titon: A PHP Modular Framework';
    protected $lipsum = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi eget tellus nibh.';

    public function testCharAt() {
        $this->assertEquals('T', String::charAt('Titon', 0));
        $this->assertEquals('i', String::charAt('Titon', 1));
        $this->assertEquals('t', String::charAt('Titon', 2));
        $this->assertEquals('o', String::charAt('Titon', 3));
        $this->assertEquals('n', String::charAt('Titon', 4));
        $this->assertEquals(null, String::charAt('Titon', 5));
        $this->assertEquals(null, String::charAt('Titon', -1));
    }

    public function testCompare() {
        $this->assertEquals('0', String::compare('foo', 'foo'));

        $this->assertLessThan('0', String::compare('foo', 'Foobar'));
        $this->assertGreaterThan('0', String::compare('foobar', 'Foo'));

        $this->assertEquals('0', String::compare('foo', 'Foobar', 3));
        $this->assertGreaterThan('0', String::compare('foobar', 'Foo', 5));
    }

    public function testContains() {
        $this->assertTrue(String::contains($this->string, 'Titon'));
        $this->assertFalse(String::contains($this->string, 'Zend'));

        // case-insensitive
        $this->assertTrue(String::contains($this->string, 'TITON', false));

        // offset
        $this->assertFalse(String::contains($this->string, 'Titon', true, 5));
    }

    public function testEndsWith() {
        $this->assertTrue(String::endsWith($this->string, 'work'));
        $this->assertFalse(String::endsWith($this->string, 'works'));

        // case-insensitive
        $this->assertTrue(String::endsWith($this->string, 'WORK', false));
        $this->assertFalse(String::endsWith($this->string, 'WORKS', false));
    }

    public function testExtract() {
        $this->assertEquals('Titon: A PHP Modular Framework', String::extract($this->string, 0));
        $this->assertEquals('Titon', String::extract($this->string, 0, 5));
        $this->assertEquals('Framework', String::extract($this->string, -9));
        $this->assertEquals('Frame', String::extract($this->string, -9, 5));
        $this->assertEquals('Modular', String::extract($this->string, 13, 7));
    }

    public function testGenerate() {
        $this->assertNotEquals('ABCDEFGHIJ', String::generate(10));
        $this->assertTrue(strlen(String::generate(10)) == 10);

        $this->assertNotEquals('ABCDEFGHIJ12345', String::generate(15));
        $this->assertTrue(strlen(String::generate(15)) == 15);

        $this->assertEquals('aaaaa', String::generate(5, 'aaaaa'));
        $this->assertTrue(strlen(String::generate(5)) == 5);
    }

    public function testIndexOf() {
        $this->assertEquals(0, String::indexOf($this->string, 'T'));
        $this->assertEquals(2, String::indexOf($this->string, 't'));
        $this->assertEquals(7, String::indexOf($this->string, 'A'));
        $this->assertEquals(13, String::indexOf($this->string, 'M'));
        $this->assertEquals(21, String::indexOf($this->string, 'F'));

        // case-insensitive
        $this->assertEquals(0, String::indexOf($this->string, 'T', false));
        $this->assertEquals(0, String::indexOf($this->string, 't', false));

        // offset
        $this->assertEquals(3, String::indexOf($this->string, 'o'));
        $this->assertEquals(14, String::indexOf($this->string, 'o', true, 5));
    }

    public function testInsert() {
        $this->assertEquals('Titon is the best PHP framework around!', String::insert('{framework} is the best {lang} framework around!', [
            'framework' => 'Titon',
            'lang' => 'PHP'
        ]));

        $this->assertEquals('Titon is the best PHP framework around!', String::insert(':framework is the best :lang framework around!', [
            'framework' => 'Titon',
            'lang' => 'PHP'
        ], [
            'before' => ':',
            'after' => ''
        ]));
    }

    public function testLastIndexOf() {
        $this->assertEquals(0, String::lastIndexOf($this->string, 'T'));
        $this->assertEquals(2, String::lastIndexOf($this->string, 't'));
        $this->assertEquals(7, String::lastIndexOf($this->string, 'A'));
        $this->assertEquals(13, String::lastIndexOf($this->string, 'M'));
        $this->assertEquals(21, String::lastIndexOf($this->string, 'F'));

        // case-insensitive
        $this->assertEquals(2, String::lastIndexOf($this->string, 'T', false));
        $this->assertEquals(24, String::lastIndexOf($this->string, 'M', false));
        $this->assertEquals(24, String::lastIndexOf($this->string, 'm', false));

        // offset
        $this->assertEquals(27, String::lastIndexOf($this->string, 'o'));
        $this->assertEquals(27, String::lastIndexOf($this->string, 'o', true, 5));
    }

    public function testListing() {
        $this->assertEquals('red, blue &amp; green', String::listing(['red', 'blue', 'green']));
        $this->assertEquals('red &amp; green', String::listing(['red', 'green']));
        $this->assertEquals('blue', String::listing(['blue']));
        $this->assertEquals('green', String::listing('green'));

        // custom
        $this->assertEquals('red, blue, and green', String::listing(['red', 'blue', 'green'], ', and '));
        $this->assertEquals('red - blue and green', String::listing(['red', 'blue', 'green'], ' and ', ' - '));
    }

    public function testShorten() {
        $this->assertEquals('Lorem &hellip; nibh.', String::shorten($this->lipsum, 10));
        $this->assertEquals('Lorem ipsum &hellip; tellus nibh.', String::shorten($this->lipsum, 25));
        $this->assertEquals('Lorem ipsum dolor sit &hellip; Morbi eget tellus nibh.', String::shorten($this->lipsum, 50));
        $this->assertEquals($this->lipsum, String::shorten($this->lipsum, 100));

        // custom
        $this->assertEquals('Lorem ... nibh.', String::shorten($this->lipsum, 10, ' ... '));
    }

    public function testStartsWith() {
        $this->assertTrue(String::startsWith($this->string, 'Titon'));
        $this->assertFalse(String::startsWith($this->string, 'Titan'));

        // case-insensitive
        $this->assertTrue(String::startsWith($this->string, 'TITON', false));
        $this->assertFalse(String::startsWith($this->string, 'TITAN', false));
    }

    public function testTruncate() {
        $string = 'This has <a href="/" class="link">anchor tags</a> &amp; entities in it. It has &quot;quotes&quot; as well.';

        // Preserve HTML and word
        $this->assertEquals('This has <a href="/" class="link">anchor</a>&hellip;', String::truncate($string, 15));
        $this->assertEquals('This has <a href="/" class="link">anchor tags</a> &amp;&hellip;', String::truncate($string, 25));
        $this->assertEquals('This has <a href="/" class="link">anchor tags</a> &amp; entities in&hellip;', String::truncate($string, 35));
        $this->assertEquals('This has <a href="/" class="link">anchor tags</a> &amp; entities in it. It&hellip;', String::truncate($string, 45));
        $this->assertEquals('This has <a href="/" class="link">anchor tags</a> &amp; entities in it. It has &quot;quotes&quot; as well.', String::truncate($string, false));

        // Preserve HTML
        $this->assertEquals('This has <a href="/" class="link">anch</a>&hellip;', String::truncate($string, 13, ['word' => false]));
        $this->assertEquals('This has <a href="/" class="link">anchor tags</a> &amp; en&hellip;', String::truncate($string, 25, ['word' => false]));
        $this->assertEquals('This has <a href="/" class="link">anchor tags</a> &amp; entities in&hellip;', String::truncate($string, 35, ['word' => false]));
        $this->assertEquals('This has <a href="/" class="link">anchor tags</a> &amp; entities in it. It has&hellip;', String::truncate($string, 45, ['word' => false]));
        $this->assertEquals('This has <a href="/" class="link">anchor tags</a> &amp; entities in it. It has &quot;quo&hellip;', String::truncate($string, 50, ['word' => false]));

        // Preserve none
        $this->assertEquals('This has anchor tags &amp; en&hellip;', String::truncate($string, 25, ['word' => false, 'html' => false]));
        $this->assertEquals('This has anchor tags &amp; entities in&hellip;', String::truncate($string, 35, ['word' => false, 'html' => false]));
        $this->assertEquals('This has anchor tags &amp; entities in it. It has&hellip;', String::truncate($string, 45, ['word' => false, 'html' => false]));
        $this->assertEquals('This has anchor tags &amp; entities in it. It has &quot;quotes&quot; as well.', String::truncate($string, false, ['word' => false, 'html' => false]));

        // Preserve none with custom suffix
        $this->assertEquals('This has anchor tags &amp; en...', String::truncate($string, 25, ['word' => false, 'html' => false, 'suffix' => '...']));
        $this->assertEquals('This has anchor tags &amp; entities in...', String::truncate($string, 35, ['word' => false, 'html' => false, 'suffix' => '...']));
        $this->assertEquals('This has anchor tags &amp; entities in it. It has...', String::truncate($string, 45, ['word' => false, 'html' => false, 'suffix' => '...']));
        $this->assertEquals('This has anchor tags &amp; entities in it. It has &quot;quotes&quot; as well.', String::truncate($string, false, ['word' => false, 'html' => false, 'suffix' => '...']));

        // Custom tags (BB code)
        $string = 'This has [url="/"]anchor tags[/url] &amp; entities in it. It has &quot;quotes&quot; as well.';

        $this->assertEquals('This has [url="/"]anchor[/url]&hellip;', String::truncate($string, 15, ['open' => '[', 'close' => ']']));
        $this->assertEquals('This has [url="/"]anchor tags[/url] &amp;&hellip;', String::truncate($string, 25, ['open' => '[', 'close' => ']']));
        $this->assertEquals('This has [url="/"]anchor tags[/url] &amp; entities in&hellip;', String::truncate($string, 35, ['open' => '[', 'close' => ']']));
        $this->assertEquals('This has [url="/"]anchor tags[/url] &amp; entities in it. It&hellip;', String::truncate($string, 45, ['open' => '[', 'close' => ']']));
        $this->assertEquals('This has [url="/"]anchor tags[/url] &amp; entities in it. It has &quot;quotes&quot; as well.', String::truncate($string, false, ['open' => '[', 'close' => ']']));
    }

    public function testUuid() {
        $this->assertRegExp('/^[a-f0-9]{8}\-[a-f0-9]{4}\-4[a-f0-9]{3}\-[89AB]{1}[a-f0-9]{3}\-[a-f0-9]{12}$/i', String::uuid());
        $this->assertRegExp('/^[a-f0-9]{8}\-[a-f0-9]{4}\-4[a-f0-9]{3}\-[89AB]{1}[a-f0-9]{3}\-[a-f0-9]{12}$/i', String::uuid());
        $this->assertRegExp('/^[a-f0-9]{8}\-[a-f0-9]{4}\-4[a-f0-9]{3}\-[89AB]{1}[a-f0-9]{3}\-[a-f0-9]{12}$/i', String::uuid());
    }

}