<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper;

use Titon\Model\Model;
use Titon\Utility\Config;
use Titon\Utility\Inflector;
use Titon\Utility\Col;
use Titon\Utility\Path;

/**
 * The FormHelper is used for HTML form creation. Data is passed to the associated input fields
 * if a value is present with the request ($_POST, $_GET and $_FILES) or a model.
 *
 * @package Titon\View\Helper
 */
class FormHelper extends AbstractHelper {

    /**
     * Configuration.
     *
     * @type Map<string, mixed> {
     *      @type string $dayFormat     Format to display days in
     *      @type string $monthFormat   Format to display months in
     *      @type string $yearFormat    Format to display years in
     *      @type string $24hour        Display all times in 24 hour formats
     * }
     */
    protected Map<string, mixed> $_config = Map {
        'dayFormat' => 'j',
        'monthFormat' => 'F',
        'yearFormat' => 'Y',
        '24hour' => true
    };

    /**
     * Number of forms generated for the current request.
     *
     * @type int
     */
    protected int $_forms = 0;

    /**
     * Fields that have failed validation.
     *
     * @type Vector<string>
     */
    protected Vector<string> $_invalid = Vector {};

    /**
     * The model currently being used to generate a form.
     *
     * @type \Titon\Model\Model
     */
    protected ?Model $_model;

    /**
     * A list of all HTML tags used within the current helper.
     *
     * @type Map<string, string>
     */
    protected Map<string, string> $_tags = Map {
        'input'             => '<input{attr}>',
        'textarea'          => '<textarea{attr}>{body}</textarea>',
        'label'             => '<label{attr}>{body}</label>',
        'select'            => '<select{attr}>{body}</select>',
        'option'            => '<option{attr}>{body}</option>',
        'optgroup'          => '<optgroup{attr}>{body}</optgroup>',
        'button'            => '<button{attr}>{body}</button>',
        'legend'            => '<legend>{body}</legend>',
        'form_open'         => '<form{attr}>',
        'form_close'        => '</form>',
        'fieldset_open'     => '<fieldset>',
        'fieldset_close'    => '</fieldset>'
    };

    /**
     * Parses an array of attributes to the HTML equivalent.
     *
     * @param Map<string, mixed> $attributes
     * @param Vector<string> $remove
     * @return string
     */
    public function attributes(Map<string, mixed> $attributes, Vector<string> $remove = Vector {}): string {
        $remove->addAll(Vector {
            'defaultDay', 'dayFormat', 'defaultHour', 'defaultMeridiem', 'defaultSecond',
            'defaultMinute', 'defaultMonth', 'monthFormat', 'options', 'default',
            'defaultYear', 'yearFormat', 'reverseYear', 'startYear', 'endYear'
        });

        return parent::attributes($attributes, $remove);
    }

    /**
     * Create a binary checkbox that accepts a 0 or 1 value.
     * Uses a hidden input field as a fallback since checkbox values aren't sent in the request
     * when not checked.
     *
     * @param string $name
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function binary(string $name, Map<string, mixed> $attributes = Map {}): string {

        // Don't use input() since the `value` attribute will change
        $hidden = $this->tag('input', Map {
            'attr' => $this->attributes(Map {
                'value' => 0,
                'type' => 'hidden',
                'name' => $this->formatName($name),
                'id' => $this->formatID($name . '.hidden')
            })
        });

        return $hidden . $this->checkbox($name, 1, $attributes);
    }

    /**
     * Create a single checkbox element.
     *
     * @param string $name
     * @param mixed $value
     * @param Map<string, mixed> $attributes {
     *      @type string $default   The checkbox to be selected by default
     * }
     * @return string
     */
    public function checkbox(string $name, mixed $value = 1, Map<string, mixed> $attributes = Map {}): string {
        $checked = $this->getValue($name, $attributes);

        if ($this->isChecked($checked, $value)) {
            $attributes['checked'] = true;
        }

        return $this->input($name, $value, 'checkbox', $attributes);
    }

    /**
     * Create an array of multiple checkboxes using the option values.
     *
     * @param string $name
     * @param Vector<mixed> $options
     * @param Map<string, mixed> $attributes {
     *      @type string $default   The checkbox to be selected by default
     * }
     * @return Vector<string>
     */
    public function checkboxes(string $name, Vector<mixed> $options, Map<string, mixed> $attributes = Map {}): Vector<string> {
        $checkboxes = Vector {};
        $id = $this->formatID($name);

        foreach ($options as $option) {
            $attribs = $attributes->toMap(); // Clone it
            $attribs['id'] = $id . '-' . Inflector::slug($option);
            $attribs['name'] = $name . '.[]';

            $checkboxes[] = $this->checkbox($name, $option, $attribs);
        }

        return $checkboxes;
    }

    /**
     * Close a form by outputting the form close tag. If the submit button text or legend is present, append those elements.
     *
     * @return string
     */
    public function close(): string {
        $this->_model = null;

        return $this->tag('form_close');
    }

    /**
     * Get a value from the attributes if it exists, else check the Helper config, and lastly return the default if nothing was found.
     *
     * @param string $key
     * @param Map<string, mixed> $attributes
     * @param mixed $default
     * @return mixed
     */
    public function config(string $key, Map<string, mixed> $attributes, mixed $default = null): mixed {
        if (isset($attributes[$key])) {
            return $attributes[$key];

        } else if ($this->hasConfig($key)) {
            return $this->getConfig($key);
        }

        return $default;
    }

    /**
     * Create a select dropdown for a calendar date: month, day, year.
     *
     * @param string $name
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function date(string $name, Map<string, mixed> $attributes = Map {}): string {
        $map = Map {'name' => $name . '.year'};
        $year = $this->year($name . '.year', $map->setAll($attributes));

        $map = Map {'name' => $name . '.month'};
        $month = $this->month($name . '.month', $map->setAll($attributes));

        $map = Map {'name' => $name . '.day'};
        $day = $this->day($name . '.day', $map->setAll($attributes));

        return $month . ' ' . $day . ' ' . $year;
    }

    /**
     * Combine both date() and time() to output all datetime related dropdowns.
     *
     * @param string $name
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function dateTime(string $name, Map<string, mixed> $attributes = Map {}): string {
        return $this->date($name, $attributes) . ' - ' . $this->time($name, $attributes);
    }

    /**
     * Create a select dropdown for calendar days, with a range of 1-31.
     *
     * @param string $name
     * @param Map<string, mixed> $attributes {
     *      @type string $dayFormat How the day should be formatted
     *      @type int $defaultDay   The default selected day
     * }
     * @return string
     */
    public function day(string $name, Map<string, mixed> $attributes = Map {}): string {
        $format = $this->config('dayFormat', $attributes, 'j');
        $options = Map {};

        for ($i = 1; $i <= 31; ++$i) {
            $v = $i;

            if ($format === 'd') {
                $v = str_pad($v, 2, '0', STR_PAD_LEFT);
            }

            $options[$i] = $v;
        }

        if (isset($attributes['defaultDay'])) {
            $attributes['default'] = $attributes['defaultDay'];
        }

        return $this->select($name, $options, $attributes);
    }

    /**
     * Create an email input field.
     *
     * @param string $name
     * @param mixed $value
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function email(string $name, mixed $value = null, Map<string, mixed> $attributes = Map {}): string {
        return $this->input($name, $value, 'email', $attributes);
    }

    /**
     * Create a file upload and browse input field.
     *
     * @param string $name
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function file(string $name, Map<string, mixed> $attributes = Map {}): string {
        return $this->input($name, null, 'file', $attributes);
    }

    /**
     * Format a dot notated path into an HTML/CSS ID name.
     *
     * @param string $name
     * @return string
     */
    public function formatID(string $name): string {
        if ($model = $this->getModel()) {
            $name = Path::className(get_class($model)) . '.' . $name;
        }

        $parts = explode('.', $name);
        $id = Vector {};

        foreach ($parts as $part) {
            if ($part !== '') {
                $id[] = Inflector::slug($part);
            }
        }

        return implode('-', $id);
    }

    /**
     * Format a dot notated path into an input name attribute.
     *
     * @param string $name
     * @return string
     */
    public function formatName(string $name): string {
        $parts = explode('.', $name);
        $path = array_shift($parts);

        foreach ($parts as $part) {
            if ($part === '') {
                continue;
            } else if (strpos($part, '[') === false) {
                $path .= '[' . $part . ']';
            } else {
                $path .= $part;
            }
        }

        return $path;
    }

    /**
     * Find and return a default value that has been defined in the attributes.
     *
     * @param Map<string, mixed> $attributes
     * @param Vector<string> $keys
     * @return mixed
     */
    public function getDefaultValue(Map<string, mixed> $attributes, Vector<string> $keys = Vector {'default'}): mixed {
        foreach ($keys as $key) {
            if (isset($attributes[$key])) {
                return $attributes[$key];
            }
        }

        return null;
    }

    /**
     * Return the current model.
     *
     * @return \Titon\Model\Model
     */
    public function getModel(): ?Model {
        return $this->_model;
    }

    /**
     * Find and return a value from the request.
     *
     * @param string $key
     * @return mixed
     */
    public function getRequestValue(string $key): mixed {
        $request = $this->getRequest();

        // Use the data bag on the request object
        if ($request && isset($request->data)) {
            $data = $request->data;

        // Fallback to the globals
        } else {
            $data = array_merge($_GET, $_POST);
        }

        return Col::extract($data, $key);
    }

    /**
     * Find and return a value from the request, or from a default value if no request is found.
     *
     * @param string $name
     * @param Map<string, mixed> $attributes
     * @return mixed
     */
    public function getValue(string $name, Map<string, mixed> $attributes = Map {}): mixed {
        $value = $this->getRequestValue($name);
        $model = $this->getModel();

        // Fetch from model attributes directly
        if ($value === null && $model) {
            $value = Col::extract($model->toArray(), $name);
        }

        // Fallback to default value
        if ($value === null) {
            $value = $this->getDefaultValue($attributes);
        }

        return $value;
    }

    /**
     * Create a hidden input field.
     *
     * @param string $name
     * @param mixed $value
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function hidden(string $name, mixed $value = null, Map<string, mixed> $attributes = Map {}): string {
        return $this->input($name, $value, 'hidden', $attributes);
    }

    /**
     * Create a select dropdown for hours, with a range of 0-23, or 1-12.
     *
     * @param string $name
     * @param Map<string, mixed> $attributes {
     *      @type int $defaultHour  The default selected hour
     * }
     * @return string
     */
    public function hour(string $name, Map<string, mixed> $attributes = Map {}): string {
        $options = Map {};

        if ($this->getConfig('24hour')) {
            $start = 0;
            $end = 23;
        } else {
            $start = 1;
            $end = 12;
        }

        for ($i = $start; $i <= $end; ++$i) {
            $options[$i] = sprintf('%02d', $i);
        }

        if (isset($attributes['defaultHour'])) {
            $attributes['default'] = $attributes['defaultHour'];
        }

        return $this->select($name, $options, $attributes);
    }

    /**
     * Create a standard input field that auto-populates with the correct request value.
     *
     * @param string $name
     * @param mixed $value
     * @param string $type
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function input(string $name, mixed $value = null, string $type = 'text', Map<string, mixed> $attributes = Map {}): string {
        $attributes = $this->prepareAttributes(Map {'name' => $name, 'value' => $value, 'type' => $type}, $attributes);

        // Grab current value from the request
        $requestValue = $this->getValue($name, $attributes);

        // Set the value attribute on non-radio/checkbox since those use `checked` attribute
        if ($requestValue !== null && !in_array($type, ['radio', 'checkbox'])) {
            $attributes['value'] = $requestValue;
        }

        // Don't set value attribute for files or password
        if ($type === 'file' || $type === 'password') {
            unset($attributes['value']);
        }

        return $this->tag('input', Map {
            'attr' => $this->attributes($attributes)
        });
    }

    /**
     * Return true if the checkbox/radio should be checked.
     *
     * @param mixed $checked    The checked value from the request, or the fallback default value
     * @param mixed $value      The value for the checkbox
     * @return bool
     */
    public function isChecked(mixed $checked, mixed $value): bool {
        if ($checked === null) {
            return false;
        }

        return (
            // Default checked
            is_bool($checked) && $checked ||
            // Multiple values
            $checked instanceof Traversable && in_array($value, $checked) ||
            // Current value equals the base value
            $checked == $value
        );
    }

    /**
     * Return true if the option in the select should be selected.
     *
     * @param mixed $selected   The selected value from the request, or the fallback default value
     * @param mixed $value      The value for hte option
     * @return bool
     */
    public function isSelected(mixed $selected, mixed $value): bool {
        if ($selected === null) {
            return false;
        }

        return (
            // Multiple values
            $selected instanceof Traversable && in_array($value, $selected) ||
            // Current value equals the base value
            $selected == $value
        );
    }

    /**
     * Create a label form for an input field.
     *
     * @param string $name
     * @param string $title
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function label(string $name, string $title, Map<string, mixed> $attributes = Map {}): string {
        $attributes = (Map {'for' => $this->formatID($name)})->setAll($attributes);

        return $this->tag('label', Map {
            'attr' => $this->attributes($attributes),
            'body' => $this->escape($title, $this->config('escape', $attributes, true))
        });
    }

    /**
     * Create a select dropdown for a time meridiem.
     *
     * @param string $name
     * @param Map<string, mixed> $attributes {
     *      @type string $defaultMeridiem   The default selected meridiem
     * }
     * @return string
     */
    public function meridiem(string $name, Map<string, mixed> $attributes = Map {}): string {
        if (isset($attributes['defaultMeridiem'])) {
            $attributes['default'] = $attributes['defaultMeridiem'];
        }

        return $this->select($name, Map {'am' => 'AM', 'pm' => 'PM'}, $attributes);
    }

    /**
     * Create a select dropdown for minutes, with a range of 1-60.
     *
     * @param string $name
     * @param Map<string, mixed> $attributes {
     *      @type int $defaultMinute    The default selected minute
     * }
     * @return string
     */
    public function minute(string $name, Map<string, mixed> $attributes = Map {}): string {
        $options = Map {};

        for ($i = 0; $i <= 59; ++$i) {
            $options[$i] = sprintf('%02d', $i);
        }

        if (isset($attributes['defaultMinute'])) {
            $attributes['default'] = $attributes['defaultMinute'];
        }

        return $this->select($name, $options, $attributes);
    }

    /**
     * Set a model for the form and open the form.
     *
     * @param \Titon\Model\Model $model
     * @param string $action
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function model(Model $model, string $action, Map<string, mixed> $attributes = Map {}): string {
        $this->setModel($model);

        return $this->open($action, $attributes);
    }

    /**
     * Create a select dropdown for calendar months, with a range of 1-12.
     *
     * @param string $name
     * @param Map<string, mixed> $attributes {
     *      @type string $monthFormat   Format the month names in the dropdown
     *      @type int $defaultMonth     The default selected month
     * }
     * @return string
     */
    public function month(string $name, Map<string, mixed> $attributes = Map {}): string {
        $format = $this->config('monthFormat', $attributes, 'F');
        $options = Map {};
        $year = date('Y');

        for ($i = 1; $i <= 12; ++$i) {
            $options[$i] = date($format, mktime(0, 0, 0, $i, 1, $year));
        }

        if (isset($attributes['defaultMonth'])) {
            $attributes['default'] = $attributes['defaultMonth'];
        }

        return $this->select($name, $options, $attributes);
    }

    /**
     * Open a form by outputting the form open tag.
     *
     * @param string $action
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function open(string $action, Map<string, mixed> $attributes = Map {}): string {
        $this->_forms++;

        if (isset($attributes['type'])) {
            if ($attributes['type'] === 'file') {
                $attributes['enctype'] = 'multipart/form-data';
                $attributes['method'] = 'post';
            }

            unset($attributes['type']);
        }

        $attributes = Col::merge(Map {
            'accept-charset' => Config::encoding(),
            'enctype' => 'application/x-www-form-urlencoded',
            'method' => 'post',
            'action' => $action,
            'id' => $this->formatID('form.' . $this->_forms)
        }, $attributes);

        $attributes['method'] = strtoupper($attributes['method']);

        return $this->tag('form_open', Map {
            'attr' => $this->attributes($attributes)
        });
    }

    /**
     * Create an option element for a select drop down.
     *
     * @param string $label
     * @param mixed $value
     * @param mixed $selected
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function option(string $label, mixed $value, mixed $selected = null, Map<string, mixed> $attributes = Map {}): string {
        $attributes['value'] = $value;

        if ($this->isSelected($selected, $value)) {
            $attributes['selected'] = 'selected';
        }

        return $this->tag('option', Map {
            'attr' => $this->attributes($attributes),
            'body' => $this->escape($label)
        });
    }

    /**
     * Create an optgroup element with a nested list of options.
     *
     * @param string $label
     * @param Map<mixed, mixed> $options
     * @param mixed $selected
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function optionGroup(string $label, Map<mixed, mixed> $options, mixed $selected = null, Map<string, mixed> $attributes = Map {}): string {
        $attributes['label'] = $label;
        $output = PHP_EOL;

        foreach ($options as $value => $option) {
            $output .= $this->option($option, $value, $selected);
        }

        return $this->tag('optgroup', Map {
            'attr' => $this->attributes($attributes),
            'body' => $output
        });
    }

    /**
     * Create a password input field.
     *
     * @param string $name
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function password(string $name, Map<string, mixed> $attributes = Map {}): string {
        return $this->input($name, null, 'password', $attributes);
    }

    /**
     * Parse all the default and required attributes that are used within the input field.
     *
     * @param Map<string, mixed> $defaults
     * @param Map<string, mixed> $attributes
     * @return array
     */
    public function prepareAttributes(Map<string, mixed> $defaults = Map {}, Map<string, mixed> $attributes = Map {}) {
        $attributes = Col::merge($defaults, $attributes);
        $namePath = $attributes['name'];

        if (!isset($attributes['id'])) {
            $attributes['id'] = $this->formatID($namePath);
        }

        $attributes['name'] = $this->formatName($namePath);

        // Change multiple=true to multiple=multiple
        $remove = Vector {};

        foreach ($attributes as $key => $value) {
            if ($key === 'empty') {
                continue;

            } else if ($value === true) {
                $attributes[$key] = $key;

            } else if ($value === false) {
                $remove[] = $key;
            }
        }

        foreach ($remove as $key) {
            unset($attributes[$key]);
        }

        return $attributes;
    }

    /**
     * Create a single radio input element.
     *
     * @param string $name
     * @param mixed $value
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function radio(string $name, mixed $value, Map<string, mixed> $attributes = Map {}): string {
        $checked = $this->getValue($name, $attributes);

        if ($this->isChecked($checked, $value)) {
            $attributes['checked'] = true;
        }

        return $this->input($name, $value, 'radio', $attributes);
    }

    /**
     * Create a multiple radio buttons using the array of option values.
     *
     * @param string $name
     * @param Vector<mixed> $options
     * @param Map<string, mixed> $attributes {
     *      @type string default    The radio to be selected by default
     * }
     * @return Vector<string>
     */
    public function radios(string $name, Vector<mixed> $options, Map<string, mixed> $attributes = Map {}): Vector<string> {
        $radios = Vector {};
        $id = $this->formatID($name);

        foreach ($options as $option) {
            $attribs = $attributes->toMap(); // Clone it
            $attribs['id'] = $id . '-' . Inflector::slug($option);

            $radios[] = $this->radio($name, $option, $attribs);
        }

        return $radios;
    }

    /**
     * Create a form reset button.
     *
     * @param string $title
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function reset(string $title, Map<string, mixed> $attributes = Map {}): string {
        $attributes = Col::merge(Map {
            'id' => $this->formatID('form.' . $this->_forms . '.reset'),
            'type' => 'reset'
        }, $attributes);

        return $this->tag('button', Map {
            'attr' => $this->attributes($attributes),
            'body' => $this->escape($title, $this->config('escape', $attributes, true))
        });
    }

    /**
     * Create a select dropdown for seconds, with a range of 0-59.
     *
     * @param string $name
     * @param Map<string, mixed> $attributes {
     *      @type int $defaultSecond    The default selected second
     * }
     * @return string
     */
    public function second(string $name, Map<string, mixed> $attributes = Map {}): string {
        $options = Map {};

        for ($i = 0; $i <= 59; ++$i) {
            $options[$i] = sprintf('%02d', $i);
        }

        if (isset($attributes['defaultSecond'])) {
            $attributes['default'] = $attributes['defaultSecond'];
        }

        return $this->select($name, $options, $attributes);
    }

    /**
     * Create a select list with values based off an options array.
     *
     * @param string $name
     * @param Map<mixed, mixed> $options
     * @param Map<string, mixed> $attributes {
     *      @type mixed $default    The option to be selected by default
     *      @type string $empty     Display an empty option at the top of the list
     *      @type bool $multiple    Set to true to allow multiple selection
     * }
     * @return string
     */
    public function select(string $name, Map<mixed, mixed> $options, Map<string, mixed> $attributes = Map {}): string {
        $attributes = $this->prepareAttributes(Map {'name' => $name}, $attributes);
        $output = PHP_EOL;
        $selected = $this->getValue($name, $attributes);

        // Prepend with an empty option
        if (isset($attributes['empty'])) {
            $empty = ($attributes['empty'] === true) ? '' : $attributes['empty'];
            $output .= $this->option($empty, '', $selected);
        }

        // Generate the options
        foreach ($options as $value => $option) {
            if ($option instanceof Traversable) {
                $output .= $this->optionGroup($value, $option, $selected);
            } else {
                $output .= $this->option((string) $option, $value, $selected);
            }
        }

        return $this->tag('select', Map {
            'attr' => $this->attributes($attributes, Vector {'value', 'empty'}),
            'body' => $output
        });
    }

    /**
     * Set the model.
     *
     * @param \Titon\Model\Model $model
     * @return $this
     */
    public function setModel(Model $model): this {
        $this->_model = $model;

        return $this;
    }

    /**
     * Create a form submit button.
     *
     * @param string $title
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function submit(string $title, Map<string, mixed> $attributes = Map {}): string {
        $attributes = Col::merge(Map {
            'id' => $this->formatID('form.' . $this->_forms . '.submit'),
            'type' => 'submit'
        }, $attributes);

        return $this->tag('button', Map {
            'attr' => $this->attributes($attributes),
            'body' => $this->escape($title, $this->config('escape', $attributes, true))
        });
    }

    /**
     * Create a basic input text field.
     *
     * @param string $name
     * @param mixed $value
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function text(string $name, mixed $value = null, Map<string, mixed> $attributes = Map {}): string {
        return $this->input($name, $value, 'text', $attributes);
    }

    /**
     * Create a textarea field and determine the correct value content.
     *
     * @param string $name
     * @param mixed $value
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function textarea(string $name, mixed $value = null, Map<string, mixed> $attributes = Map {}): string {
        $attributes = $this->prepareAttributes(Map {'name' => $name, 'cols' => 25, 'rows' => 5}, $attributes);
        $requestValue = $this->getRequestValue($name);

        if ($requestValue !== null) {
            $value = $requestValue;
        }

        return $this->tag('textarea', Map {
            'attr' => $this->attributes($attributes, Vector {'value'}),
            'body' => $value
        });
    }

    /**
     * Create multiple select dropdowns for hours, minutes, seconds and the meridiem.
     *
     * @param string $name
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function time(string $name, Map<string, mixed> $attributes = Map {}): string {
        $map = Map {'name' => $name . '.hour'};
        $hour = $this->hour($name . '.hour', $map->setAll($attributes));

        $map = Map {'name' => $name . '.minute'};
        $minute = $this->minute($name . '.minute', $map->setAll($attributes));

        $map = Map {'name' => $name . '.second'};
        $second = $this->second($name . '.second', $map->setAll($attributes));

        $output = $hour . ':' . $minute . ':' . $second;

        if (!$this->getConfig('24hour')) {
            $map = Map {'name' => $name . '.meridiem'};
            $output .= $this->meridiem($name . '.meridiem', $map->setAll($attributes));
        }

        return $output;
    }

    /**
     * Generate a CSRF protection token field.
     *
     * @param string $value
     * @return string
     */
    public function token(string $value): string {
        return $this->hidden('_token', $value);
    }

    /**
     * Create a url input field.
     *
     * @param string $name
     * @param mixed $value
     * @param Map<string, mixed> $attributes
     * @return string
     */
    public function url(string $name, mixed $value = null, Map<string, mixed> $attributes = Map {}): string {
        return $this->input($name, $value, 'url', $attributes);
    }

    /**
     * Create a select dropdown for calendar years, with a user defined range.
     *
     * @param string $name
     * @param Map<string, mixed> $attributes {
     *      @type int $startYear        The year to start the range
     *      @type int $endYear          The year to end the range
     *      @type bool $reverseYear     Should the years be in reverse order
     *      @type string $yearFormat    How the year should be formatted
     *      @type int $defaultYear      The default selected year
     * }
     * @return string
     */
    public function year(string $name, Map<string, mixed> $attributes = Map {}): string {
        $month = date('n');
        $year = date('Y');
        $reverse = $this->config('reverseYear', $attributes, false);
        $format = $this->config('yearFormat', $attributes, 'Y');
        $start = $this->config('startYear', $attributes, $year);
        $end = $this->config('endYear', $attributes, ($year + 10));
        $options = Map {};

        for ($i = $start; $i <= $end; ++$i) {
            $options[$i] = date($format, mktime(0, 0, 0, $month, 1, $i));
        }

        if ($reverse) {
            $options = array_reverse($options, true);
        }

        if (isset($attributes['defaultYear'])) {
            $attributes['default'] = $attributes['defaultYear'];
        }

        return $this->select($name, new Map($options), $attributes);
    }

}