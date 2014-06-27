<?php
namespace Titon\Cache\Storage;

class XcacheStorageTest extends AbstractStorageTest {

    protected function setUp() {
        if (!extension_loaded('xcache')) {
            $this->markTestSkipped('Xcache is not installed or configured properly');
        }

        $this->object = new XcacheStorage();

        parent::setUp();
    }

}