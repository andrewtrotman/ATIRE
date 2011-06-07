/**
 * OB3 DocumentTree extends Ext.Panel
 * 
 * @copyright GHC Holdings Ltd 2010, All Rights Reserved
 * @package ob3
 * @subpackage none
 */

OB3.DocumentTree = Ext.extend(Ext.tree.TreePanel, {
	
	id: 'ob3-tree',
	
	border: false,
	
	useArrows: true,
	
	autoScroll: true,
	
	animate: true,
	
	containerScroll: true,
	
	border: false,
	
	autoExpand: true,
	
	enableDD: true,
	
	ddAppendOnly: true,
	
	ddScroll: true,
	
	rootVisible: true,
	
	root: {
		expandable: true,
		expanded: true,
		draggable: false,
		id: 'root',
		nodeType: 'ob3-root',
		text: 'My Documents',
		cls: 'folder'
	},
	
	initComponent: function() {
		
		//add toolbar buttons
		this.tbar = [{
			text: 'Create New',	
			iconCls: 'ob3-icon-create-new',
			menu: {
				items: [{
					text: 'Document',
					iconCls: 'ob3-icon-new-document',
					handler: this.createDocument,
					scope: this
				}, {
					text: 'Folder',
					iconCls: 'ob3-icon-new-folder',
					handler: this.createFolder,
					scope: this
				}, {
					text : 'Search',
					iconCls : 'ob3-icon-new-document',
					handler: this.createSearch,
					scope: this
				}]
			}
		}];
		
		OB3.DocumentTree.superclass.initComponent.call(this);
		
		this.on('render', function() { this.initEl(this.el); });
	},
	
	initEl: function(el) {},
	
	/**
	 * Create document
	 */
	createDocument: function() {
		var document = Ext.create({
			xtype: 'OB3.Document',
			values: [
				{field: 'Name', value: 'New Document'},
				{field: 'Title', value: 'New Document'},
			]
		});
		
		var treeNode = OB3.Viewport.getDocumentTree().insertChild({
			id: 'ob3-treenode-'+new Date().getTime(),
			text: document.title,
			nodeType: 'ob3-document',
			selected: true,
			leaf: true
		});
		
		treeNode.setValue('Name', 'New Document');
		treeNode.setValue('Title', 'New Document');
		
		//relationships
		treeNode.setDocument(document);
		document.setTreeNode(treeNode);
		
		document.on('createsuccess', function() {
			treeNode.setValue('ID', document.getValue('ID'));
		});
		
		//add to browser
		OB3.Viewport.getCenter().add(document);
		
		//add default resources
		document.addResource('OB3.RichHeading', {
			values: [
				{field: 'Value', value: 'New Document'},
				{field: 'Level', value: 1}
			],
			autoEdit: false,
			autoRemove: false,
			autoSelect: false
		});
		
		document.addResource('OB3.RichParagraph');
		
		document.write();
	},
	
	/**
	 * Create Folder
	 */
	createFolder: function() {
		var folder = OB3.Viewport.getDocumentTree().insertChild({
			text: 'New Folder',
			nodeType: 'ob3-folder',
			selected: true,
			cls: 'folder'
		});
		
		folder.setValue('Name', 'New Folder');

		folder.write();
	},

	createSearch : function(btn) {
		//If we have created the "searchDocument", then grab it. 
		//TODO: Allow multiple search document. Option 1: Search document 
		//      manager. Option 2. Putting search combobox into center panel.
		var search = Ext.getCmp('searchDocument');
		
		if(!search){
			var searchComboConfig = {
				xtype : 'combo',
				id : 'searchInput',
				hiddenName: 'ID',
				displayField: 'term',
				triggerAction: 'all',
				pageSize: 5,
				resizable: false,
				minListWidth:200,
				minChars:2,
				enableKeyEvents : true,
				store: new Ext.data.DirectStore({
					storeId: 'searchInputStore',
					api: {
						read: OB3.API.Search.getSuggestions
					},
					paramOrder:['query','start','limit'],	
					paramsAsHash: false,
					autoSave: true,
					reader: new Ext.data.JsonReader({
						idProperty: 'ID',
						root: 'values',
						totalProperty: 'count',
						fields:[
							{name:'term'}
						]
					})
				}),
				listeners: {
					
					//on select an item in the drop down. 
					select: function(combo,record,index){
						sendSearchQuery(Ext.getCmp("searchDocument"),
										new Array(record.data.term),combo);
					},
					
					//on key enter from the text field. 
					keypress: function(combo,e){
						var terms;
						if (e.getCharCode() == e.ENTER){
							
							//stop the process of querying the server for auto 
							//completion.
							combo.on('beforequery',function (e){
														e.cancel = true;
												},this);
							terms = combo.getRawValue();
							
						//if combo text field contains at least one character. 
							if(/\S/.test(terms)){
								sendSearchQuery(Ext.getCmp("searchDocument"),
												new Array(terms),combo);
								
								//re-enable querying server for auto completion
								combo.on('beforequery',function (e){
														e.cancel = false;
												},this);
							}
						}
					}
				}
			};
	
			/*
			 * Function  clean the current content of the search document and 
			 * send the query. 
			 * 
			 * $param searchDocument The search document to add the result
			 * $param searchTerms The entire search query. It could be multiple 
			 *                    words. Here is not protection taken on 
			 *                    validating the query. 
			 * $param combo The text field.
			 * 
			 */
			
			var sendSearchQuery = function (searchDocument,searchTerms,combo) {
				var searchBody = searchDocument.getBody();
				var searchTop = searchBody.get('top');
							
				combo.clearValue();
				
				//cleaning the last search results
				while (search.resultCount > 0){
					searchBody.removeAt(search.resultCount);
					search.resultCount -= 1;
				}
				
				
				searchTop.setTitle("Searching for");
				searchTop.setSubtitle(searchTerms);
				searchDocument.setTitle(searchTerms);
				searchDocument.initTopEl();
		
				//TODO: Enable paganation of the search result?
				// Here "1" is the offset of the entire search result set
				// "10" is the page size
				OB3.API.Search.doSearch(searchTerms,1,10,function(response, e) {
					//We could remove event searh, I think...
					if(this.fireEvent('search', response, e) !== false) {
						if(e.type == 'exception') {
							this.fireEvent('updatefailure', response, e);
						}
						else if(this.fireEvent('updatesuccess', response, e) !==
								false) {
							this.initEl(this.el);
						}
					}
				}, searchDocument);
			};
			
			var search_document_on_update_success =	function(response) {
				
				//We are handling when a result is double clicked. 
				var search_result_on_dbl_click = function() {
					var document;
					var find_by_ID_and_highlight = function (document, ID){
						var resultSet = document.findBy(
										function(component,container){
											if (component.className === 'DocumentNode' &&
												component.getResource())
												if (component.getResource().getField('ID').value == ID)
													return true;
										});
												
						if (resultSet[0]){
							resultSet[0].setSelected(true);
						}
					};
										
					//We want to open the document from the tree node. 
					Ext.getCmp('ob3-tree').loader.getByID(this.documentID).open();
					
					//Then we use the tree node to get the docuemnt once it is opened. 
					document = Ext.getCmp('ob3-tree').loader.getByID(this.documentID).getDocument();
					
					//FIXME: We do not want to use defer function here. We do 
					//not guarantee that a long document is going to finish 
					//adding all its nodes by 1000 millis. We prefer event based
					//approach.
					find_by_ID_and_highlight.defer(1000,null,[document,this.resourceID])
				};
				
				var search_result_response_handler = function(result) {
					
					var tmp=this.addResource(
								'OB3.' + result.values[1].value,
								{
									autoEdit : false,
									autoSelect : false,
									canEdit : false,
									droppable : false,
									values : result.values,
									has_one : result.has_one,
									has_many : result.has_many
							});
							
					tmp.documentID = result.values.pop().value;
					tmp.resourceID = result.values[0].value;
								
					//Double clicked on a result node
					tmp.on('dblclick', search_result_on_dbl_click, tmp);
				
					tmp.clearChanges();
				
					this.resultCount += 1;
				};
				
				this.resultCount = 0;
				
				Ext.each(response.values, search_result_response_handler, this);
			
				return true;
			};
				
		 	search = Ext.create({
					xtype : 'OB3.Document',
					
					//FIXME:I need to find it to add result in.Later this should
					//		be removed for multiple search documents.
					id : 'searchDocument',
					title : 'Search',
					resultCount : 0,
					canEdit : false,
					values : [{
							field : 'Name',
							value : 'Search'
						}],
					tbar : ['->', '<b>Search:</b>',
							searchComboConfig,// Combo Box for auto completion
							{	
								xtype : 'button',
								text : 'Search',
								handler : function() {
									var input = Ext.getCmp('searchInput');
									var search = Ext.getCmp('searchDocument');
						 			var data = new Array(input.getRawValue());
									
						 			sendSearchQuery(search,data,input);
								}
								
					}]

			});
		
			
			//Search results come back from server. 
			search.on('updatesuccess', search_document_on_update_success, search);
			
			OB3.Viewport.getCenter().add(search);
			
		}else{
			search.setActive(true);
		}
	},
	
	/**
	 * Insert a tree node at a certain point.
	 */
	insertChild: function(attributes, path) {
		var sn = path ? this.loader.get(path) : this.getSelectionModel().selNode;
	
		if(sn) {
			if(sn.leaf) {
				var newNode = this.loader.appendChild(sn.parentNode.getPath(), attributes);
			}
			else {
				var newNode = this.loader.appendChild(sn.getPath(), attributes);
				if(!sn.isExpanded() && this.autoExpand) sn.expand();
			}
		}
		else {
			var newNode = this.loader.appendChild(this.root.getPath(), attributes);
			if(!this.root.isExpanded() && this.autoExpand) this.root.expand();
		}
		
		if(newNode && attributes.selected) newNode.select();
		
		return newNode;
	},
	
	loadAll: function() {
		Ext.MessageBox.wait('Loading, please wait...', 'Loading');
		
		OB3.API.DocumentTree.loadAll(function(nodes){
			Ext.each(nodes, function(response) {
				var attributes = response.values;
				
				var node = {};
				Ext.each(attributes, function(attribute) {
					node[attribute.field] = attribute.value;
				});
				
				var attributes = {
					text: node.Name,
					id: node.ID,
					nodeType: 'ob3-'+node.Type,
					cls: node.Type
				};
				
				if(node.Type == 'document') attributes.leaf = true;
				
				//parent or root
				var parent = (node.ParentNodeID > 0) ? this.loader.getByID(node.ParentNodeID) : this.root; 
					
				if((newNode = this.loader.appendChild(parent.getPath(), attributes))) {
					newNode.setValue('ID', node.ID);
					newNode.setValue('Name', node.Name);
					newNode.setValue('Title', node.Title);
					newNode.setValue('Subtitle', node.Subtitle);
					newNode.clearChanges(); //reset changes
				}
			}, this);
			
			Ext.MessageBox.hide();
		}, this);
	}
});

Ext.reg('OB3.DocumentTree', OB3.DocumentTree);