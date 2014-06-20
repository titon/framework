<?php
namespace Titon\Utility;

use Titon\Test\TestCase;

class PathTest extends TestCase {

    public function testAlias() {
        $this->assertEquals('[internal]', Path::alias(null));
        $this->assertEquals('[vendor]Titon' . DS . 'Debug' . DS . 'Debugger.php', Path::alias(VENDOR_DIR . '/Titon/Debug/Debugger.php'));
        $this->assertEquals('[src]Titon' . DS . 'Debug' . DS . 'Debugger.php', Path::alias(dirname(TEST_DIR) . '/src/Titon/Debug/Debugger.php'));
        $this->assertEquals('[app]some' . DS . 'file.txt', Path::alias('/app/some/file.txt', array('app' => '/app')));
    }

    public function testClassName() {
        $this->assertEquals('ClassName', Path::className('\test\namespace\ClassName'));
        $this->assertEquals('ClassName', Path::className('test:namespace:ClassName', ':'));
        $this->assertEquals('ClassName', Path::className('test/namespace/ClassName', '/'));
        $this->assertEquals('ClassName', Path::className('test.namespace.ClassName', '.'));
    }

    public function testPackageName() {
        $this->assertEquals('test\namespace', Path::packageName('\test\namespace\ClassName'));
        $this->assertEquals('test/namespace', Path::packageName('/test/namespace/ClassName', '/'));
    }

    public function testDs() {
        // linux
        $this->assertEquals(DS . 'some' . DS . 'fake' . DS . 'folder' . DS . 'path' . DS . 'fileName.php', Path::ds('/some/fake/folder/path/fileName.php'));
        $this->assertEquals(DS . 'some' . DS . 'fake' . DS . 'folder' . DS . 'path' . DS . 'fileName.php', Path::ds('/some\fake/folder\path/fileName.php'));

        // windows
        $this->assertEquals('C:' . DS . 'some' . DS . 'fake' . DS . 'folder' . DS . 'path' . DS . 'fileName.php', Path::ds('C:\some\fake\folder\path\fileName.php'));
        $this->assertEquals('C:' . DS . 'some' . DS . 'fake' . DS . 'folder' . DS . 'path' . DS . 'fileName.php', Path::ds('C:\some/fake\folder/path\fileName.php'));

        // linux
        $this->assertEquals(DS . 'some' . DS . 'fake' . DS . 'folder' . DS . 'path' . DS . 'fileName' . DS, Path::ds('/some/fake/folder/path/fileName', true));
        $this->assertEquals(DS . 'some' . DS . 'fake' . DS . 'folder' . DS . 'path' . DS . 'fileName' . DS, Path::ds('/some\fake/folder\path/fileName/', true));

        // windows
        $this->assertEquals('C:' . DS . 'some' . DS . 'fake' . DS . 'folder' . DS . 'path' . DS . 'fileName' . DS, Path::ds('C:\some\fake\folder\path\fileName/'));
        $this->assertEquals('C:' . DS . 'some' . DS . 'fake' . DS . 'folder' . DS . 'path' . DS . 'fileName' . DS, Path::ds('C:\some/fake\folder/path\fileName\\'));
    }

    public function testIncludePath() {
        $baseIncludePath = get_include_path();
        $selfPath1 = '/fake/test/1';
        $selfPath2 = '/fake/test/2';
        $selfPath3 = '/fake/test/3';

        $this->assertEquals($baseIncludePath, get_include_path());

        Path::includePath($selfPath1);
        $this->assertEquals($baseIncludePath . PATH_SEPARATOR . $selfPath1, get_include_path());

        Path::includePath(array($selfPath2, $selfPath3));
        $this->assertEquals($baseIncludePath . PATH_SEPARATOR . $selfPath1 . PATH_SEPARATOR . $selfPath2 . PATH_SEPARATOR . $selfPath3, get_include_path());
    }

    public function testIsAbsolute() {
        $this->assertTrue(Path::isAbsolute('/root/path'));
        $this->assertTrue(Path::isAbsolute('\root\path'));
        $this->assertTrue(Path::isAbsolute('C:\root\path'));
        $this->assertTrue(Path::isAbsolute('abc123:\root\path'));

        $this->assertFalse(Path::isAbsolute('sub/'));
        $this->assertFalse(Path::isAbsolute('./sub/'));
        $this->assertFalse(Path::isAbsolute('../sub/'));
    }

    public function testIsRelative() {
        $this->assertFalse(Path::isRelative('/root/path'));
        $this->assertFalse(Path::isRelative('\root\path'));
        $this->assertFalse(Path::isRelative('C:\root\path'));
        $this->assertFalse(Path::isRelative('abc123:\root\path'));

        $this->assertTrue(Path::isRelative('sub/'));
        $this->assertTrue(Path::isRelative('./sub/'));
        $this->assertTrue(Path::isRelative('../sub/'));
    }

    public function testJoin() {
        $this->assertEquals('foo' . DS . 'bar', Path::join(array('foo', 'bar')));
        $this->assertEquals('foo' . DS . 'bar', Path::join(array('foo/', '/bar/')));
        $this->assertEquals('foo' . DS . 'baz', Path::join(array('foo/', '/bar/', '..', '//baz')));
        $this->assertEquals('baz', Path::join(array('foo/', '/bar/', '..', '..', '//baz')));
        $this->assertEquals('..' . DS . 'baz', Path::join(array('foo/', '..', '/bar', '.', '..', '..', '//baz')));
        $this->assertEquals('baz', Path::join(array('foo/', '..', '/bar', '.', '..', '..', '//baz'), false));
        $this->assertEquals('foo' . DS . 'bar' . DS . 'foo' . DS . 'a' . DS . 'b' . DS . 'c' . DS . 'e', Path::join(array('foo', '.', 'bar\\baz', '..', 'foo', '.', 'a/b/c', 'd/../e')));
        $this->assertEquals(array('foo', 'baz'), Path::join(array('foo/', '/bar/', '..', '//baz'), true, false));
    }

    /**
     * @expectedException \Titon\Utility\Exception\InvalidTypeException
     */
    public function testJoinErrorsOnNonString() {
        Path::join(array('foo', 123));
    }

    public function testRelativeTo() {
        $this->assertEquals('.' . DS, Path::relativeTo('/foo/bar', '/foo/bar'));
        $this->assertEquals('..' . DS, Path::relativeTo('/foo/bar', '/foo'));
        $this->assertEquals('.' . DS . 'baz' . DS, Path::relativeTo('/foo/bar', '/foo/bar/baz'));
        $this->assertEquals('..' . DS . '..' . DS . '..' . DS . '..' . DS . 'd' . DS . 'e' . DS . 'f' . DS, Path::relativeTo('/foo/bar/a/b/c', '/foo/d/e/f'));
    }

    /**
     * @expectedException \Titon\Utility\Exception\InvalidArgumentException
     */
    public function testRelativeToErrorsOnAbsolute() {
        Path::relativeTo('/abs', '../rel');
    }

    public function testStripExt() {
        $this->assertEquals('NoExt', Path::stripExt('NoExt'));
        $this->assertEquals('ClassName', Path::stripExt('ClassName.php'));
        $this->assertEquals('File_Name', Path::stripExt('File_Name.php'));

        $this->assertEquals('\test\namespace\ClassName', Path::stripExt('\test\namespace\ClassName.php'));
        $this->assertEquals('\test\namespace\Class_Name', Path::stripExt('\test\namespace\Class_Name.php'));

        $this->assertEquals('/test/file/path/FileName', Path::stripExt('/test/file/path/FileName.php'));
        $this->assertEquals('/test/file/path/File/Name', Path::stripExt('/test/file/path/File/Name.php'));
    }

    public function testToNamespace() {
        $this->assertEquals('test\file\path\FileName', Path::toNamespace('/test/file/path/FileName.php'));
        $this->assertEquals('test\file\path\File\Name', Path::toNamespace('/test/file/path/File/Name.php'));

        $this->assertEquals('test\file\path\FileName', Path::toNamespace('vendors/src/test/file/path/FileName.php'));
        $this->assertEquals('Titon\test\file\path\File\Name', Path::toNamespace('vendors/src/Titon/test/file/path/File/Name.php'));
    }

    public function testToPath() {
        $this->assertEquals(DS . 'test' . DS . 'namespace' . DS . 'ClassName.php', Path::toPath('\test\namespace\ClassName'));
        $this->assertEquals(DS . 'test' . DS . 'namespace' . DS . 'Class' . DS . 'Name.php', Path::toPath('\test\namespace\Class_Name'));

        $this->assertEquals(DS . 'Test' . DS . 'NameSpace' . DS . 'ClassName.php', Path::toPath('\Test\NameSpace\ClassName'));
        $this->assertEquals(DS . 'Test' . DS . 'NameSpace' . DS . 'Class' . DS . 'Name.php', Path::toPath('\Test\NameSpace\Class_Name'));

        $this->assertEquals(DS . 'test' . DS . 'namespace' . DS . 'ClassName.PHP', Path::toPath('\test\namespace\ClassName', 'PHP'));
        $this->assertEquals(DS . 'test' . DS . 'namespace' . DS . 'Class' . DS . 'Name.PHP', Path::toPath('\test\namespace\Class_Name', 'PHP'));

        $this->assertEquals(TEST_DIR . DS . 'test' . DS . 'namespace' . DS . 'ClassName.php', Path::toPath('\test\namespace\ClassName', 'php', TEST_DIR));
        $this->assertEquals(TEST_DIR . DS . 'test' . DS . 'namespace' . DS . 'Class' . DS . 'Name.php', Path::toPath('\test\namespace\Class_Name', 'php', TEST_DIR));

        $this->assertEquals(VENDOR_DIR . DS . 'test' . DS . 'namespace' . DS . 'ClassName.php', Path::toPath('\test\namespace\ClassName', 'php', VENDOR_DIR));
        $this->assertEquals(VENDOR_DIR . DS . 'test' . DS . 'namespace' . DS . 'Class' . DS . 'Name.php', Path::toPath('\test\namespace\Class_Name', 'php', VENDOR_DIR));
    }

}