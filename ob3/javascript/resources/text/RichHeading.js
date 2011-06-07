/**
 * OB3 Heading Resource
 * 
 * @copyright GHC Holdings Ltd 2010, All Rights Reserved
 * @package ob3
 * @subpackage none
 */

OB3.RichHeading = Ext.extend(OB3.RichText, {
	
	/**
	 * @see OB3.DataComponent.className
	 */
	className: 'RichHeading',

	/**
	 * @see OB3.DataComponent.fields
	 */
	fields: [
		{field: 'Level', type: 'Int'}
	],	
	
	/**
	 * @private {String} _title
	 */
	title: _('OB3.Resource.Text.Heading.title'),
	
	/**
	 * @private {String} nodeClass
	 */
	nodeClass: 'heading',
	
	//init editor
	initEditor: function(editor) {
		OB3.RichHeading.superclass.initEditor.call(this, editor); //call parent
		
		editor.field.on('keydown', this.onEditorKeyDown, this);
	},
	
	/**
	 * Fires when any key related to navigation (arrows, tab, enter, esc, etc.) is pressed.  You can check
	 * {@link Ext.EventObject#getKey} to determine which key was pressed.
	 * @param {Ext.form.Field} this
	 * @param {Ext.EventObject} e The event object
	 */
	onEditorKeyDown: function(e) {
		var key = e.getKey(),
		node = this.getNode(),
		document = node.getDocument(),
		editor = this.getEditor(),
		cp = editor.getCaretPos(),
		value = editor.getValue();
		
		if(key == e.ENTER) {
			var value = OB3.Encoder.htmlDecode(value);
				str1 = value.substr(0, cp).replace(/\s{1,}/i, ' ').replace(/^\s{1,}/i, ''),
				str2 = value.substr(cp).replace(/\s{1,}/i, ' ').replace(/^\s{1,}/i, '');
				
			this.getEditor().onStopTyping.cancel();
			
			editor.setValue(str1);
			
			var node = document.addResource('OB3.RichParagraph', {
				values: [{field: 'Value', value: str2}],
				caretPos: 0
			});
		}
		else if(key == e.BACKSPACE && cp == 0) {
			var prevNode = node.getPreviousNode(),
				prevResource = prevNode.getResource();
				
			if(prevResource.className === 'RichParagraph' || prevResource.className === 'RichHeading') {
				var prevEditor = prevResource.getEditor(),
					curValue = prevResource.getValue('Value');

				prevNode.setSelected(true);
				prevResource.setValue('Value', curValue+value);
				prevResource.startEdit(curValue.length);
				
				this.delete();
				
				e.preventDefault();
				e.stopPropagation();
			}
		}
	},
	
	//get node class
	getNodeClass: function() {
		return 'heading-h'+this.getValue('Level');
	},
	
	/**
	 * Generates the html for the resource
	 * @return {String} html string
	 */
	getHTML: function() {
		var dom = Ext.DomHelper.markup({
			tag: 'h'+this.getValue('Level'),
			html: this.getValue('Value')
		});
		
		return dom;
	},

	/**
	 * Gets an array of items for the resource toolbar
	 * @return {Array} an array of items for the resource toolbar
	 */
	getToolbarItems: function() {
		var items = OB3.RichHeading.superclass.getToolbarItems.call(this);
		
		if(!this.canEdit) return items //If this heading is not editable, we want just return whatever parent has for the toolbar.
		
		if(items.length) items.push({xtype: 'tbseparator'});
		
		items.push({xtype: 'tbitem', cls: 'ob3-toolbar-title', html: 'Style'});
		
		//heading dropdown
		items.push({
			xtype: 'combo',
			width: 100,
			store: [
					['1', 'Heading 1'],
					['2', 'Heading 2'],
					['3', 'Heading 3'],
					['4', 'Heading 4']
				],
				triggerAction: 'all',
				forceSelection: true,
				listeners: {
				'select': function(combo) {				
					this.setValue('Level', combo.value);
					this.getNode().write(null, null, true);
				},
				 scope: this
			},
			value: this.getValue('Level')
		});
		
		 return items;
	},
	
	//apply classes no level change
	__setLevel: function(level) {
		var editor = this.getEditor();		

		var newLevel = level; //new level	
		var oldLevel = this.getValue('Level'); //old level
		
		this.getNode().el.replaceClass(
			'ob3-node-heading-h'+oldLevel,
			'ob3-node-heading-h'+newLevel
		);
		
		this.getNode().getResourceHolder().el.replaceClass(
			'ob3-resource-holder-heading-h'+oldLevel,
			'ob3-resource-holder-heading-h'+newLevel
		);
		
		this.el.replaceClass(
			'ob3-resource-heading-h'+oldLevel,
			'ob3-resource-heading-h'+newLevel
		);
		
		editor.setIFrameBodyClass('heading-h'+newLevel);
		editor.autoSize(); //resize editor to match size
		
		return level;
	},
	
	//add a default level
	__getLevel: function(value) {
		if(value > 1) {
			return value;
		}
		else return 1;
	},
});

OB3.RichText.registerType({
	className: 'Heading',
	xtype: 'OB3.RichHeading',
	attributes: ['Level'],
	patterns: [/^<h(\d+)[^>]*>/i],
	matches: ['Level']
});

Ext.reg('OB3.RichHeading', OB3.RichHeading);