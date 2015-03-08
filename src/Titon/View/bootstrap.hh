<?hh
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

/**
 * --------------------------------------------------------------
 *  Type Aliases
 * --------------------------------------------------------------
 *
 * Defines type aliases that are used by the view package.
 */

namespace Titon\View {
    type DataMap = Map<string, mixed>;
    type HelperMap = Map<string, Helper>;
    type LocaleList = Vector<string>;
    type PathList = Vector<string>;
}

namespace Titon\View\Engine {
    type WrapperList = Vector<string>;
}

namespace Titon\View\Helper {
    type AttributeMap = Map<string, mixed>;
    type TagMap = Map<string, string>;
    type OptionMap = Map<string, mixed>;

    // Asset
    type Asset = shape('path' => string, 'env' => string, 'attributes' => AttributeMap);
    type ScriptMap = Map<string, Map<int, Asset>>;
    type StyleSheetMap = Map<int, Asset>;

    // Block
    type ActiveBlockList = Vector<string>;
    type BlockMap = Map<string, string>;

    // Breadcrumb
    type Breadcrumb = shape('title' => string, 'url' => string, 'attributes' => AttributeMap);
}
