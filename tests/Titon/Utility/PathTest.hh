<?hh // strict
namespace Titon\Utility;

use Titon\Test\TestCase;

class PathTest extends TestCase {

    public function testAlias(): void {
        $this->assertEquals('[internal]', Path::alias(''));
        $this->assertEquals('[vendor]Titon/Debug/Debugger.php', Path::alias(VENDOR_DIR . '/Titon/Debug/Debugger.php'));
        $this->assertEquals('[src]Titon/Debug/Debugger.php', Path::alias(dirname(TEST_DIR) . '/src/Titon/Debug/Debugger.php'));
        $this->assertEquals('[app]some/file.txt', Path::alias('/app/some/file.txt', Map {'app' => '/app'}));
    }

    public function testClassName(): void {
        $this->assertEquals('ClassName', Path::className('\test\namespace\ClassName'));
        $this->assertEquals('ClassName', Path::className('test:namespace:ClassName', ':'));
        $this->assertEquals('ClassName', Path::className('test/namespace/ClassName', '/'));
        $this->assertEquals('ClassName', Path::className('test.namespace.ClassName', '.'));
    }

    public function testPackageName(): void {
        $this->assertEquals('test\namespace', Path::packageName('\test\namespace\ClassName'));
        $this->assertEquals('test/namespace', Path::packageName('/test/namespace/ClassName', '/'));
    }

    public function testDs(): void {
        // linux
        $this->assertEquals('/some/fake/folder/path/fileName.php', Path::ds('/some/fake/folder/path/fileName.php'));
        $this->assertEquals('/some/fake/folder/path/fileName.php', Path::ds('/some\fake/folder\path/fileName.php'));

        // windows
        $this->assertEquals('C:/some/fake/folder/path/fileName.php', Path::ds('C:\some\fake\folder\path\fileName.php'));
        $this->assertEquals('C:/some/fake/folder/path/fileName.php', Path::ds('C:\some/fake\folder/path\fileName.php'));

        // linux
        $this->assertEquals('/some/fake/folder/path/fileName/', Path::ds('/some/fake/folder/path/fileName', true));
        $this->assertEquals('/some/fake/folder/path/fileName/', Path::ds('/some\fake/folder\path/fileName/', true));

        // windows
        $this->assertEquals('C:/some/fake/folder/path/fileName/', Path::ds('C:\some\fake\folder\path\fileName/'));
        $this->assertEquals('C:/some/fake/folder/path/fileName/', Path::ds('C:\some/fake\folder/path\fileName\\'));
    }

    public function testIncludePath(): void {
        $baseIncludePath = get_include_path();
        $selfPath1 = '/fake/test/1';
        $selfPath2 = '/fake/test/2';
        $selfPath3 = '/fake/test/3';

        $this->assertEquals($baseIncludePath, get_include_path());

        Path::includePath(Vector {$selfPath1});
        $this->assertEquals($baseIncludePath . PATH_SEPARATOR . $selfPath1, get_include_path());

        Path::includePath(Vector {$selfPath2, $selfPath3});
        $this->assertEquals($baseIncludePath . PATH_SEPARATOR . $selfPath1 . PATH_SEPARATOR . $selfPath2 . PATH_SEPARATOR . $selfPath3, get_include_path());
    }

    public function testIsAbsolute(): void {
        $this->assertTrue(Path::isAbsolute('/root/path'));
        $this->assertTrue(Path::isAbsolute('\root\path'));
        $this->assertTrue(Path::isAbsolute('C:\root\path'));
        $this->assertTrue(Path::isAbsolute('abc123:\root\path'));

        $this->assertFalse(Path::isAbsolute('sub/'));
        $this->assertFalse(Path::isAbsolute('./sub/'));
        $this->assertFalse(Path::isAbsolute('../sub/'));
    }

    public function testIsRelative(): void {
        $this->assertFalse(Path::isRelative('/root/path'));
        $this->assertFalse(Path::isRelative('\root\path'));
        $this->assertFalse(Path::isRelative('C:\root\path'));
        $this->assertFalse(Path::isRelative('abc123:\root\path'));

        $this->assertTrue(Path::isRelative('sub/'));
        $this->assertTrue(Path::isRelative('./sub/'));
        $this->assertTrue(Path::isRelative('../sub/'));
    }

    public function testJoin(): void {
        $this->assertEquals('foo/bar', Path::join(Vector {'foo', 'bar'}));
        $this->assertEquals('foo/bar', Path::join(Vector {'foo/', '/bar/'}));
        $this->assertEquals('foo/baz', Path::join(Vector {'foo/', '/bar/', '..', '//baz'}));
        $this->assertEquals('baz', Path::join(Vector {'foo/', '/bar/', '..', '..', '//baz'}));
        $this->assertEquals('../baz', Path::join(Vector {'foo/', '..', '/bar', '.', '..', '..', '//baz'}));
        $this->assertEquals('baz', Path::join(Vector {'foo/', '..', '/bar', '.', '..', '..', '//baz'}, false));
        $this->assertEquals('foo/bar/foo/a/b/c/e', Path::join(Vector {'foo', '.', 'bar\\baz', '..', 'foo', '.', 'a/b/c', 'd/../e'}));
        $this->assertEquals(Vector {'foo', 'baz'}, Path::join(Vector {'foo/', '/bar/', '..', '//baz'}, true, false));
    }

    public function testRelativeTo(): void {
        $this->assertEquals('./', Path::relativeTo('/foo/bar', '/foo/bar'));
        $this->assertEquals('../', Path::relativeTo('/foo/bar', '/foo'));
        $this->assertEquals('./baz/', Path::relativeTo('/foo/bar', '/foo/bar/baz'));
        $this->assertEquals('../../../../d/e/f/', Path::relativeTo('/foo/bar/a/b/c', '/foo/d/e/f'));
    }

    /**
     * @expectedException \Titon\Common\Exception\InvalidArgumentException
     */
    public function testRelativeToErrorsOnAbsolute(): void {
        Path::relativeTo('/abs', '../rel');
    }

    public function testStripExt(): void {
        $this->assertEquals('NoExt', Path::stripExt('NoExt'));
        $this->assertEquals('ClassName', Path::stripExt('ClassName.php'));
        $this->assertEquals('File_Name', Path::stripExt('File_Name.php'));

        $this->assertEquals('\test\namespace\ClassName', Path::stripExt('\test\namespace\ClassName.php'));
        $this->assertEquals('\test\namespace\Class_Name', Path::stripExt('\test\namespace\Class_Name.php'));

        $this->assertEquals('/test/file/path/FileName', Path::stripExt('/test/file/path/FileName.php'));
        $this->assertEquals('/test/file/path/File/Name', Path::stripExt('/test/file/path/File/Name.php'));
    }

    public function testToNamespace(): void {
        $this->assertEquals('test\file\path\FileName', Path::toNamespace('/test/file/path/FileName.php'));
        $this->assertEquals('test\file\path\File\Name', Path::toNamespace('/test/file/path/File/Name.php'));

        $this->assertEquals('test\file\path\FileName', Path::toNamespace('vendors/src/test/file/path/FileName.php'));
        $this->assertEquals('Titon\test\file\path\File\Name', Path::toNamespace('vendors/src/Titon/test/file/path/File/Name.php'));
    }

    public function testToPath(): void {
        $this->assertEquals('/test/namespace/ClassName.php', Path::toPath('\test\namespace\ClassName'));
        $this->assertEquals('/test/namespace/Class/Name.php', Path::toPath('\test\namespace\Class_Name'));

        $this->assertEquals('/Test/NameSpace/ClassName.php', Path::toPath('\Test\NameSpace\ClassName'));
        $this->assertEquals('/Test/NameSpace/Class/Name.php', Path::toPath('\Test\NameSpace\Class_Name'));

        $this->assertEquals('/test/namespace/ClassName.PHP', Path::toPath('\test\namespace\ClassName', 'PHP'));
        $this->assertEquals('/test/namespace/Class/Name.PHP', Path::toPath('\test\namespace\Class_Name', 'PHP'));

        $this->assertEquals(TEST_DIR . '/test/namespace/ClassName.php', Path::toPath('\test\namespace\ClassName', 'php', TEST_DIR));
        $this->assertEquals(TEST_DIR . '/test/namespace/Class/Name.php', Path::toPath('\test\namespace\Class_Name', 'php', TEST_DIR));

        $this->assertEquals(VENDOR_DIR . '/test/namespace/ClassName.php', Path::toPath('\test\namespace\ClassName', 'php', VENDOR_DIR));
        $this->assertEquals(VENDOR_DIR . '/test/namespace/Class/Name.php', Path::toPath('\test\namespace\Class_Name', 'php', VENDOR_DIR));
    }

}
